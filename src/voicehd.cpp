#include "voicehd.hpp"
#include "bsc.hpp"

#include <cstddef>

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

void voicehd(
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
