#include "hdchog.hpp"

#include <cstdint>
#include <iostream>

#include "defines.hpp"

using dist_bank_t = dist_t[HDCHOG_CLASSES];

// BSC encoding
void bsc_enc_grad_hv(
        bsc_hv_t &grad_hv,
        const cell_t &features,
        const bsc_hv_t (&ori_mem)[HOG_ORIENTATIONS],
        const bsc_hv_t (&mag_mem)[HOG_MAGNITUDES]
        ) {
    constexpr size_t BnbAccWidth = number_of_bits(HOG_ORIENTATIONS);
    bsc_bnb_acc_t<BnbAccWidth> bundle_acc;
    bsc_init_bnb_acc_t<BnbAccWidth>(bundle_acc);

    GradBnb:
    for (size_t ori = 0; ori < HOG_ORIENTATIONS; ori++) {
        // Encode
        bsc_hv_t mag_hv = mag_mem[features[ori]];
        bsc_bnb<BnbAccWidth>(bundle_acc, ori_mem[ori], mag_hv, bundle_acc);
    }
    bsc_bnb_threshold<BnbAccWidth>(grad_hv, bundle_acc, HOG_ORIENTATIONS/2);
}

void bsc_enc_mat_hv(
        bsc_hv_t &mat_hv,
        const bsc_hv_t (&cell_hvs)[HOG_CELLS],
        const bsc_hv_t (&grad_hvs)[HOG_CELLS]
        ) {
    constexpr size_t BnbAccWidth = number_of_bits(HOG_CELLS);
    bsc_bnb_acc_t<BnbAccWidth> bundle_acc;
    bsc_init_bnb_acc_t<BnbAccWidth>(bundle_acc);

    CellBnb:
    for (size_t i = 0; i < HOG_CELLS; i++) {
        // Encode
        bsc_bnb<BnbAccWidth>(bundle_acc, cell_hvs[i], grad_hvs[i], bundle_acc);
    }
    bsc_bnb_threshold<BnbAccWidth>(mat_hv, bundle_acc, HOG_CELLS/2);
}

void hdchog_bsc_enc(
        bsc_hv_t &query,
        const hog_t &features,
        const bsc_hv_t (&cell_mem)[HOG_CELLS],
        const bsc_hv_t (&ori_mem)[HOG_ORIENTATIONS],
        const bsc_hv_t (&mag_mem)[HOG_MAGNITUDES]
        ) {
    bsc_hv_t grad_hvs[HOG_CELLS];
    GradEnc:
    for (int i = 0; i < HOG_CELLS; i++) {
        bsc_enc_grad_hv(grad_hvs[i], features[i], ori_mem, mag_mem);
    }
    bsc_enc_mat_hv(query, cell_mem, grad_hvs);
}

// CGR encoding
void cgr_enc_grad_hv(
        cgr_hv_t &grad_hv,
        const cell_t &features,
        const cgr_hv_t (&ori_mem)[HOG_ORIENTATIONS],
        const cgr_hv_t (&mag_mem)[HOG_MAGNITUDES]
        ) {
    constexpr size_t BnbAccWidth = number_of_bits(HOG_ORIENTATIONS);

    cgr_bnb_acc_t<BnbAccWidth> bundle_acc;
    #pragma HLS array_partition variable=bundle_acc dim=1

    cgr_init_bnb_acc_t<BnbAccWidth>(bundle_acc);

    GradBnb:
    for (size_t ori = 0; ori < HOG_ORIENTATIONS; ori++) {
        // Encode
        cgr_bnb<BnbAccWidth>(bundle_acc, ori_mem[ori], mag_mem[features[ori]], bundle_acc);
    }
    cgr_bnb_threshold<BnbAccWidth>(grad_hv, bundle_acc);
}

void cgr_enc_mat_hv(
        cgr_hv_t &mat_hv,
        const cgr_hv_t (&cell_hvs)[HOG_CELLS],
        const cgr_hv_t (&grad_hvs)[HOG_CELLS]
        ) {
    constexpr size_t BnbAccWidth = number_of_bits(HOG_CELLS);

    cgr_bnb_acc_t<BnbAccWidth> bundle_acc;
    #pragma HLS array_partition variable=bundle_acc dim=1

    cgr_init_bnb_acc_t<BnbAccWidth>(bundle_acc);

    CellBnb:
    for (size_t i = 0; i < HOG_CELLS; i++) {
        // Encode
        cgr_bnb<BnbAccWidth>(bundle_acc, cell_hvs[i], grad_hvs[i], bundle_acc);
    }
    cgr_bnb_threshold<BnbAccWidth>(mat_hv, bundle_acc);
}

void hdchog_cgr_enc(
        cgr_hv_t &query,
        const hog_t &features,
        const cgr_hv_t (&cell_mem)[HOG_CELLS],
        const cgr_hv_t (&ori_mem)[HOG_ORIENTATIONS],
        const cgr_hv_t (&mag_mem)[HOG_MAGNITUDES]
        ) {
    cgr_hv_t grad_hvs[HOG_CELLS];
    GradEnc:
    for (int i = 0; i < HOG_CELLS; i++) {
        cgr_enc_grad_hv(grad_hvs[i], features[i], ori_mem, mag_mem);
    }
    cgr_enc_mat_hv(query, cell_mem, grad_hvs);
}

void hdchog_dp(
        const hog_t &features,
        const hv_t (&cell_mem)[HOG_CELLS],
        const hv_t (&ori_mem)[HOG_ORIENTATIONS],
        const hv_t (&mag_mem)[HOG_MAGNITUDES],
        const hv_t (&am)[HDCHOG_CLASSES],
        size_t datapath_id,
        dist_bank_t (&s_acc_dists)[SEGMENT_DATAPATHS]
        ) {
    dist_bank_t &acc_dists = s_acc_dists[datapath_id];
    hv_t query;

    //hdchog_bsc_enc(query, features, cell_mem, ori_mem, mag_mem);
    hdchog_cgr_enc(query, features, cell_mem, ori_mem, mag_mem);

    dist_bank_t dists;
    distN<HDCHOG_CLASSES>(dists, query, am);
    for (int i = 0; i < HDCHOG_CLASSES; i++) {
        #pragma HLS unroll
        acc_dists[i] += dists[i];
    }
}

void hdchog(
        class_t &pred,
        const hog_t &features,
        const cell_mem_t (&cell_mem),
        const ori_mem_t (&ori_mem),
        const mag_mem_t (&mag_mem),
        const am_t (&am)
) {
    static dist_bank_t (s_acc_dists)[SEGMENT_DATAPATHS];

    // Clean-up accumulator banks
    parallel_reset(s_acc_dists);

    HDCHOG_Segment:
    for (size_t s = 0; s < HV_SEGMENTS; s++) {
        // Compute segment distance
        hdchog_dp(
                features,
                cell_mem[s],
                ori_mem[s],
                mag_mem[s],
                am[s],
                s % SEGMENT_DATAPATHS,
                s_acc_dists
                );
    }

    // Accumulate dists of all datapaths
    dist_bank_t acc_dists;
    parallel_reset(acc_dists);
    HDCHOG_Dist_Accumulation:
    for (size_t dp = 0; dp < SEGMENT_DATAPATHS; dp++) {
        //#pragma HLS unroll // It does no seems to be advantageous to unroll this loop for small DPs
        for (size_t i = 0; i < HDCHOG_CLASSES; i++) {
            #pragma HLS unroll
            acc_dists[i] += s_acc_dists[dp][i];
        }
    }

    size_t argmin;
    parallel_argmin(argmin, acc_dists);
    pred = argmin;
}

