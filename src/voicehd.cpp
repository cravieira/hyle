#include "voicehd.hpp"
#include "bsc.hpp"
#include "defines.hpp"
#include "hls_vector.h"

#include <cstddef>
#include <cstdint>

template<size_t N>
static void _read_mem(
        //hv_t &out,
        //const std::array<hv_t, N> &mem,
        //size_t ind
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

void voicehd_enc_seg_dp(
        hls::vector<dim_t, VOICEHD_CLASSES> &out_dists,
        const feat_vec_t &features,
        const hv_t (&im)[VOICEHD_FEATURES],
        const hv_t (&cim)[VOICEHD_LEVELS],
        const hv_t (&am)[VOICEHD_CLASSES],
        hls::vector<dim_t, VOICEHD_CLASSES> &acc_dists
        ) {

    hv_t item, c_item;
    hv_t bound_hvs[VOICEHD_FEATURES];

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
    hv_t query;
    bsc_bundleN<VOICEHD_FEATURES>(query, bound_hvs);

    hls::vector<dim_t, VOICEHD_CLASSES> dists;
    bsc_distN<VOICEHD_CLASSES>(dists, query, am);
    out_dists = acc_dists + dists;
}

void voicehd_enc_seg(
        class_t &pred,
        const feat_vec_t &features,
        const hv_t (&im)[HV_SEGMENTS][VOICEHD_FEATURES],
        const hv_t (&cim)[HV_SEGMENTS][VOICEHD_LEVELS],
        const hv_t (&am)[HV_SEGMENTS][VOICEHD_CLASSES]
        ) {

    hls::vector<dim_t, VOICEHD_CLASSES> acc_dists = static_cast<dim_t>(0); // TODO: Should this buffer be static?
    VoiceHD_Segment:
    for (size_t s = 0; s < HV_SEGMENTS; s++) {
        // Compute segment distance
        voicehd_enc_seg_dp(
                acc_dists,
                features,
                im[s],
                cim[s],
                am[s],
                acc_dists
                );
    }

    // Compute argmin of distances to predict correct class
    size_t argmin = 0;
    dim_t val = acc_dists[0];
    Argmin:
    for (size_t i = 1; i < acc_dists.size(); i++) {
        if (val > acc_dists[i]) {
            val = acc_dists[i];
            argmin = i;
        }
    }
    pred = argmin;
}
