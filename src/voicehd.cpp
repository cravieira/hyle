#include "voicehd.hpp"
#include "common.hpp"
#include "defines.hpp"
#include "hls_vector.h"
#include "bsc.hpp"
#include "cgr.hpp"

#include <cstddef>
#include <cstdint>

using dist_bank_t = dist_t[VOICEHD_CLASSES];

template<size_t N, typename HV_T>
static void _read_mem(
        HV_T &out,
        const HV_T (&mem)[N],
        size_t ind
        ) {
    #pragma HLS inline

    // TODO: Will this create a register buffer for out? Or could we optimize for
    // single-cycle by removing the buffers? Are rvalue and && necessary for such
    // use case?
    out = mem[ind];
}

// BSC encoding
/**
 * @brief VoiceHD encoding using the fused bind-and-bundle operation.
 *
 * @param[out] query
 * @param[in] features Dataset input features
 * @param[in] im Reference to an Item Memory (IM) array
 * @param[in] cim Reference to a Continuous Item Memory (CIM) array
 */
void voicehd_enc_bnb(
        vsa::bsc::hv_t &query,
        const feat_vec_t &features,
        const vsa::bsc::hv_t (&im)[VOICEHD_FEATURES],
        const vsa::bsc::hv_t (&cim)[VOICEHD_LEVELS]
        ) {
#pragma HLS inline

    vsa::bsc::hv_t item, c_item;
    vsa::bsc::hv_t bound_hvs[VOICEHD_FEATURES];

    vsa::bsc::bnb_acc_t bundle_acc;
    vsa::bsc::init_bnb_acc_t(bundle_acc);

    SegmentBind:
    for (size_t channel = 0; channel < VOICEHD_FEATURES; channel++) {
        // Read memories
        // TODO: Reading items from IM is sequential. Maybe it could profit
        // from a sequential access pragma.
        _read_mem<VOICEHD_FEATURES>(item, im, channel);
        _read_mem<VOICEHD_LEVELS>(c_item, cim, features[channel]);

        // Encode
        vsa::bsc::bnb(bundle_acc, item, c_item, bundle_acc);
    }
    vsa::bsc::bnb_threshold(query, bundle_acc, VOICEHD_FEATURES/2);
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
        vsa::bsc::hv_t &query,
        const feat_vec_t &features,
        const vsa::bsc::hv_t (&im)[VOICEHD_FEATURES],
        const vsa::bsc::hv_t (&cim)[VOICEHD_LEVELS]
        ) {
#pragma HLS inline

    vsa::bsc::hv_t item, c_item;
    vsa::bsc::hv_t bound_hvs[VOICEHD_FEATURES];

    // Encode with split bind and bundle
    SegmentBind:
    for (size_t channel = 0; channel < VOICEHD_FEATURES; channel++) {
        // Read memories
        // TODO: Reading items from IM is sequential. Maybe it could profit
        // from a sequential access pragma.
        _read_mem<VOICEHD_FEATURES>(item, im, channel);
        _read_mem<VOICEHD_LEVELS>(c_item, cim, features[channel]);

        // Encode
        vsa::bsc::bind(bound_hvs[channel], item, c_item);
    }

    // Bundle
    vsa::bsc::bundleN<VOICEHD_FEATURES>(query, bound_hvs);
}

// CGR encoding
/**
 * @brief VoiceHD encoding using the fused bind-and-bundle operation.
 *
 * @param[out] query
 * @param[in] features Dataset input features
 * @param[in] im Reference to an Item Memory (IM) array
 * @param[in] cim Reference to a Continuous Item Memory (CIM) array
 */
void voicehd_enc_bnb(
        vsa::cgr::hv_t &query,
        const feat_vec_t &features,
        const vsa::cgr::hv_t (&im)[VOICEHD_FEATURES],
        const vsa::cgr::hv_t (&cim)[VOICEHD_LEVELS]
        ) {
#pragma HLS inline

    vsa::cgr::hv_t item, c_item;
    vsa::cgr::hv_t bound_hvs[VOICEHD_FEATURES];

    vsa::cgr::bnb_acc_t bundle_acc;
    vsa::cgr::init_bnb_acc_t(bundle_acc);

    SegmentBind:
    for (size_t channel = 0; channel < VOICEHD_FEATURES; channel++) {
        // Read memories
        // TODO: Reading items from IM is sequential. Maybe it could profit
        // from a sequential access pragma.
        _read_mem<VOICEHD_FEATURES>(item, im, channel);
        _read_mem<VOICEHD_LEVELS>(c_item, cim, features[channel]);

        // Encode
        vsa::cgr::bnb(bundle_acc, item, c_item, bundle_acc);
    }
    vsa::cgr::bnb_threshold(query, bundle_acc);
}

// Datapath functions - These functions should be HDC class agnostic
void voicehd_enc_seg_dp(
        const feat_vec_t &features,
        const hv_t (&im)[VOICEHD_FEATURES],
        const hv_t (&cim)[VOICEHD_LEVELS],
        const hv_t (&am)[VOICEHD_CLASSES],
        size_t datapath_id,
        dist_bank_t (&s_acc_dists)[SEGMENT_DATAPATHS]
        ) {

    dist_bank_t &acc_dists = s_acc_dists[datapath_id];
    hv_t query;

    // Encoding stage. Choose one option only!
    //voicehd_enc_vertical_unroll(query, features, im, cim);
    voicehd_enc_bnb(query, features, im, cim);

    dist_bank_t dists;
    distN<VOICEHD_CLASSES>(dists, query, am);
    for (int i = 0; i < VOICEHD_CLASSES; i++) {
        #pragma HLS unroll
        acc_dists[i] += dists[i];
    }
}

void voicehd_enc_seg(
        class_t &pred,
        const feat_vec_t &features,
        const hv_t (&im)[HV_SEGMENTS][VOICEHD_FEATURES],
        const hv_t (&cim)[HV_SEGMENTS][VOICEHD_LEVELS],
        const hv_t (&am)[HV_SEGMENTS][VOICEHD_CLASSES]
        ) {

    static dist_bank_t (s_acc_dists)[SEGMENT_DATAPATHS];

    // Clean-up accumulator banks
    parallel_reset(s_acc_dists);

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
    dist_bank_t acc_dists;
    parallel_reset(acc_dists);
    VoiceHD_Dist_Accumulation:
    for (size_t dp = 0; dp < SEGMENT_DATAPATHS; dp++) {
        //#pragma HLS unroll // It does no seems to be advantageous to unroll this loop for small DPs
        for (size_t i = 0; i < VOICEHD_CLASSES; i++) {
            #pragma HLS unroll
            acc_dists[i] += s_acc_dists[dp][i];
        }
    }

    size_t argmin;
    parallel_argmin(argmin, acc_dists);
    pred = argmin;
}
