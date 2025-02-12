#include "voicehd.hpp"
#include "bsc.hpp"
#include "defines.hpp"
#include "hls_vector.h"

#include <cstddef>
#include <cstdint>

template<size_t N>
static void _read_mem(
        hv_t &out,
        const hv_t (&mem)[N],
        size_t ind
        ) {
    #pragma HLS inline

    // TODO: Will this create a register buffer for out? Or could we optimize for
    // single-cycle by removing the buffers? Are rvalue and && necessary for such
    // use case?
    out = mem[ind];
}

/**
 * @brief VoiceHD encoding using the fused bind-and-bundle operation.
 *
 * @param[out] query
 * @param[in] features Dataset input features
 * @param[in] im Reference to an Item Memory (IM) array
 * @param[in] cim Reference to a Continuous Item Memory (CIM) array
 */
void voicehd_enc_bnb(
        hv_t &query,
        const feat_vec_t &features,
        const hv_t (&im)[VOICEHD_FEATURES],
        const hv_t (&cim)[VOICEHD_LEVELS]
        ) {
#pragma HLS inline

    hv_t item, c_item;
    hv_t bound_hvs[VOICEHD_FEATURES];

    bnb_acc_t bundle_acc = static_cast<bnb_acc_elem_t>(0);
    SegmentBind:
    for (size_t channel = 0; channel < VOICEHD_FEATURES; channel++) {
        // Read memories
        // TODO: Reading items from IM is sequential. Maybe it could profit
        // from a sequential access pragma.
        _read_mem<VOICEHD_FEATURES>(item, im, channel);
        _read_mem<VOICEHD_LEVELS>(c_item, cim, features[channel]);

        // Encode
        bsc_bnb(bundle_acc, item, c_item, bundle_acc);
    }
    bsc_bnb_threshold(query, bundle_acc, VOICEHD_FEATURES/2);
}

/**
 * @brief VoiceHD encoding using split binding and bundling operations.
 *
 * @param[out] query
 * @param[in] features Dataset input features
 * @param[in] im Reference to an Item Memory (IM) array
 * @param[in] cim Reference to a Continuous Item Memory (CIM) array
 */
void voicehd_enc_vertical_unroll(
        hv_t &query,
        const feat_vec_t &features,
        const hv_t (&im)[VOICEHD_FEATURES],
        const hv_t (&cim)[VOICEHD_LEVELS]
        ) {
#pragma HLS inline

    hv_t item, c_item;
    hv_t bound_hvs[VOICEHD_FEATURES];

    // Encode with split bind and bundle
    SegmentBind:
    for (size_t channel = 0; channel < VOICEHD_FEATURES; channel++) {
        // Read memories
        // TODO: Reading items from IM is sequential. Maybe it could profit
        // from a sequential access pragma.
        _read_mem<VOICEHD_FEATURES>(item, im, channel);
        _read_mem<VOICEHD_LEVELS>(c_item, cim, features[channel]);

        // Encode
        bsc_bind(bound_hvs[channel], item, c_item);
    }

    // Bundle
    bsc_bundleN<VOICEHD_FEATURES>(query, bound_hvs);
}

void voicehd_enc_seg_dp(
        const feat_vec_t &features,
        const hv_t (&im)[VOICEHD_FEATURES],
        const hv_t (&cim)[VOICEHD_LEVELS],
        const hv_t (&am)[VOICEHD_CLASSES],
        size_t datapath_id,
        hls::vector<dist_t, VOICEHD_CLASSES> (&s_acc_dists)[SEGMENT_DATAPATHS]
        ) {

    hls::vector<dist_t, VOICEHD_CLASSES> &acc_dists = s_acc_dists[datapath_id];
    hv_t query;

    // Encoding stage. Choose one option only!
    voicehd_enc_vertical_unroll(query, features, im, cim);
    //voicehd_enc_bnb(query, features, im, cim);

    hls::vector<dist_t, VOICEHD_CLASSES> dists;
    bsc_distN<VOICEHD_CLASSES>(dists, query, am);
    acc_dists += dists;
}

void voicehd_enc_seg(
        class_t &pred,
        const feat_vec_t &features,
        const hv_t (&im)[HV_SEGMENTS][VOICEHD_FEATURES],
        const hv_t (&cim)[HV_SEGMENTS][VOICEHD_LEVELS],
        const hv_t (&am)[HV_SEGMENTS][VOICEHD_CLASSES]
        ) {

    static hls::vector<dist_t, VOICEHD_CLASSES> s_acc_dists[SEGMENT_DATAPATHS];

    // Clean-up accumulator banks
    // TODO: Maybe unroll this loop in tcl?
    ResetAccumulators:
    for (size_t i = 0; i < SEGMENT_DATAPATHS; i++) {
        s_acc_dists[i] = static_cast<dist_t>(0);
    }

    VoiceHD_Segment:
    for (size_t s = 0; s < HV_SEGMENTS; s++) {
        // Compute segment distance
        voicehd_enc_seg_dp(
                features,
                im[s],
                cim[s],
                am[s],
                s % SEGMENT_DATAPATHS,
                s_acc_dists
                );
    }

    // Accumulate dists of all datapaths
    hls::vector<dist_t, VOICEHD_CLASSES> acc_dists = static_cast<dist_t>(0);
    VoiceHD_Dist_Accumulation:
    for (size_t dp = 0; dp < SEGMENT_DATAPATHS; dp++) {
        acc_dists += s_acc_dists[dp];
    }

    // Compute argmin of distances to predict correct class
    size_t argmin = 0;
    dist_t val = acc_dists[0];
    Argmin:
    for (size_t i = 1; i < acc_dists.size(); i++) {
        if (val > acc_dists[i]) {
            val = acc_dists[i];
            argmin = i;
        }
    }
    pred = argmin;
}
