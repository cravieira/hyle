#include "voicehd.hpp"
#include "bsc.hpp"
#include "defines.hpp"

#include <cstddef>
#include <cstdint>

template<size_t N>
static void _init_mem(
        std::array<hv_t, N> &mem
        ) {
    for (size_t i = 0; i < N; i++) {
        mem[i] = static_cast<bin_t>(0);
    }
}

template<size_t N>
static void _read_mem(
        hv_t &out,
        const std::array<hv_t, N> &mem,
        size_t ind
        ) {
    // TODO: Will this create a register buffer for out? Or could we optimize for
    // single-cycle by removing the buffers? Are rvalue and && necessary for such
    // use case?
    out = mem[ind];
}

void voicehd_top(
        class_t &pred,
        const std::array<feat_t, VOICEHD_FEATURES> &features
        ) {
    // TODO: Should the memories be defined outside the function so it is possible to write them?
    static std::array<hv_t, VOICEHD_FEATURES> im;
    static std::array<hv_t, VOICEHD_LEVELS> cim;
    static std::array<hv_t, VOICEHD_CLASSES> am;
    _init_mem(im);
    _init_mem(cim);
    _init_mem(am);

    hv_t item, c_item;
    static std::array<hv_t, VOICEHD_FEATURES> bound_hvs; // TODO: Sequential access

    Bind:
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
    static hv_t query;
    bsc_bundleN<VOICEHD_FEATURES>(query, bound_hvs);

    // Search
    size_t argmax;
    bsc_search<VOICEHD_CLASSES>(argmax, query, am);
    pred = static_cast<class_t>(argmax);
}

void voicehd_enc(
        class_t &pred,
        const std::array<feat_t, VOICEHD_FEATURES> &features,
        const std::array<hv_t, VOICEHD_FEATURES> &im,
        const std::array<hv_t, VOICEHD_LEVELS>   &cim,
        const std::array<hv_t, VOICEHD_CLASSES>  &am
        ) {
    hv_t item, c_item;
    static std::array<hv_t, VOICEHD_FEATURES> bound_hvs; // TODO: Sequential access

    Bind:
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
    static hv_t query;
    bsc_bundleN<VOICEHD_FEATURES>(query, bound_hvs);

    // Search
    size_t argmax;
    bsc_search<VOICEHD_CLASSES>(argmax, query, am);
    pred = static_cast<class_t>(argmax);
}

void voicehd_enc_seg_dp(
        hls::vector<dim_t, VOICEHD_CLASSES> &dists,
        const feat_vec_t &features,
        const std::array<hv_t, VOICEHD_FEATURES> &im,
        const std::array<hv_t, VOICEHD_LEVELS>   &cim,
        const std::array<hv_t, VOICEHD_CLASSES>  &am) {

    hv_t item, c_item;
    static std::array<hv_t, VOICEHD_FEATURES> bound_hvs; // TODO: Sequential access

    Bind:
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
    static hv_t query;
    bsc_bundleN<VOICEHD_FEATURES>(query, bound_hvs);

    bsc_distN<VOICEHD_CLASSES>(dists, query, am);
}

void voicehd_enc_seg(
        class_t &pred,
        const feat_vec_t &features,
        const hv_mem_seg_t<VOICEHD_FEATURES> &im,
        const hv_mem_seg_t<VOICEHD_LEVELS>   &cim,
        const hv_mem_seg_t<VOICEHD_CLASSES>  &am
        ) {

    hls::vector<dim_t, VOICEHD_CLASSES> acc_dists = static_cast<dim_t>(0); // TODO: Should this buffer be static?
    hls::vector<dim_t, VOICEHD_CLASSES> seg_dists = static_cast<dim_t>(0); // TODO: Should this buffer be static?
    VoiceHD_Segment:
    for (size_t s = 0; s < HV_SEGMENTS; s++) {
        // Compute segment distance
        voicehd_enc_seg_dp(
                seg_dists,
                features,
                im[s],
                cim[s],
                am[s]
                );

        // Accumulate segment distance into the accumulator
        acc_dists = acc_dists + seg_dists;
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
