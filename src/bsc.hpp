#pragma once

#include <ap_int.h>
#include <hls_vector.h>
#include <cstddef>
#include <iostream>

#include "common.hpp"
#include "defines.hpp"

// Type definitions
using bsc_hv_elem_t = ap_uint<1>;
using bsc_hv_t = hls::vector<bsc_hv_elem_t, HV_SEGMENT_SIZE>;
// An uint big enough to store the number of dimensions
using bsc_dim_t = ap_uint<number_of_bits(HV_SEGMENT_SIZE*HV_SEGMENTS)>;
// An uint big enought to store the max distance between BSC HVs.
using bsc_dist_t = ap_uint<number_of_bits(DIM)>;

// Special types used for the "Bind and Bundle" operator
template<size_t Width>
using bsc_bnb_acc_elem_t = ap_uint<Width>;
template<size_t AccWidth>
using bsc_bnb_acc_t = bsc_bnb_acc_elem_t<AccWidth>[HV_SEGMENT_SIZE];

// Non-member printer function for BSC hypervectors, i.e., hls::vector with
// binary digits
std::ostream& operator<<(std::ostream& os, const bsc_hv_t v);

// BSC
template<size_t BnbAccWidth>
void bsc_init_bnb_acc_t(bsc_bnb_acc_t<BnbAccWidth> &acc) { parallel_reset(acc); }

void bsc_bind(bsc_hv_t &out, const bsc_hv_t &a, const bsc_hv_t &b);

template<size_t N>
void bsc_bindN(bsc_hv_t &out, const bsc_hv_t (&hvs)[N]) {
    static_assert(
            N > 2,
            "Number of vectors in \"hvs\" collection must be greater than 2");

    out = hvs[0] ^ hvs[1];
    BindLoop:
    for (size_t i = 2; i < N; i++) {
        out ^= hvs[i];
    }
}

void bsc_bundle(bsc_hv_t &out, const bsc_hv_t &a, const bsc_hv_t &b, const bsc_hv_t &c);

template<size_t N>
void bsc_bundleN(bsc_hv_t &out, const bsc_hv_t (&hvs)[N]) {
    constexpr size_t acc_bits = number_of_bits(N);
    using acc_elem_t = ap_uint<acc_bits>;

    // Copy the LSB of each accumulator to the output
    BundleN_MajColumn:
    for (size_t col = 0; col < hvs[0].size(); col++) {
        acc_elem_t sum = 0;

        BundleN_AccColumn:
        for (size_t row = 0; row < N; row++) {
            sum += hvs[row][col];
        }

        const acc_elem_t threshold(N/2);
        // The use of ">" instead of ">=" makes the results equal to TorchHD
        bsc_hv_elem_t bit = sum > threshold ? 1 : 0;
        out[col] = bit;
    }
}

template<size_t AccBnbWidth>
void bsc_bnb_threshold(
        bsc_hv_t &out,
        const bsc_bnb_acc_t<AccBnbWidth> &acc,
        const bsc_bnb_acc_elem_t<AccBnbWidth> &threshold) {
    BscBnbThreshold:
    for (size_t i = 0; i < out.size(); i++) {
        #pragma HLS unroll
        bsc_hv_elem_t bit = acc[i] > threshold;
        out[i] = bit;
    }
}

template<size_t AccBnbWidth>
void bsc_bnb(
        bsc_bnb_acc_t<AccBnbWidth> &acc_out,
        const bsc_hv_t &a,
        const bsc_hv_t &b,
        const bsc_bnb_acc_t<AccBnbWidth> &acc_in
) {
    bsc_hv_t temp = a^b;
    BscBnbAcc:
    for (size_t i = 0; i < temp.size(); i++) {
        #pragma HLS unroll
        acc_out[i] = acc_in[i] + temp[i];
    }
}

void bsc_dist(bsc_dist_t &out, const bsc_hv_t &a, const bsc_hv_t &b);

template<size_t N>
void bsc_distN(
        bsc_dist_t (&dists)[N],
        const bsc_hv_t &query,
        const bsc_hv_t (&am)[N]
        ) {
    // TODO: Using a sub function might not be the best for the HLS tool to create
    // an optimized design, but it is the easiest implementation at the moment.
    // Revisit this function implementation when optimizing code.

    ComputeDist:
    for (size_t i = 0; i < N; i++) {
        bsc_dist(dists[i], query, am[i]);
    }
}

template<size_t N>
void bsc_search(
        size_t &argmin,
        const bsc_hv_t &query,
        const bsc_hv_t (&am)[N]
        ) {
    static_assert(N > 0, "The size of the associative memory must be greater than 1");
    // TODO: Using a sub function might not be the best for the HLS tool to create
    // an optimized design, but it is the easiest implementation at the moment.
    // Revisit this function implementation when optimizing code.

    bsc_dist_t dists[N];
    bsc_distN(dists, query, am);

    //using ind_t = ap_uint<number_of_bits(N)>;
    argmin = 0;
    bsc_dist_t val = dists[0];
    Argmin:
    for (size_t i = 1; i < N; i++) {
        if (val > dists[i]) {
            val = dists[i];
            argmin = i;
        }
    }
}

namespace vsa {
namespace bsc {

using hv_elem_t = bsc_hv_elem_t;
using hv_t = bsc_hv_t;
using dim_t = bsc_dim_t;
using dist_t = bsc_dist_t;

// Special types used for the "Bind and Bundle" operator
template<size_t AccWidth>
using bnb_acc_elem_t = bsc_bnb_acc_elem_t<AccWidth>;
template<size_t AccWidth>
using bnb_acc_t = bsc_bnb_acc_t<AccWidth>;

template<size_t BnbAccWidth>
void inline init_bnb_acc_t(bnb_acc_t<BnbAccWidth> &acc) { bsc_init_bnb_acc_t<BnbAccWidth>(acc); };

// BSC
void inline bind(hv_t &out, const hv_t &a, const hv_t &b) { bsc_bind(out, a, b); }

template<size_t N>
void inline bindN(hv_t &out, const hv_t (&hvs)[N]) { bsc_bindN(out, hvs); }

void inline bundle(hv_t &out, const hv_t &a, const hv_t &b, const hv_t &c) {bsc_bundle(out, a, b, c); }

template<size_t N>
void inline bundleN(hv_t &out, const hv_t (&hvs)[N]) { bsc_bundleN(out, hvs); }

template<size_t BnbAccWidth>
void inline bnb_threshold(
        hv_t &out,
        const bnb_acc_t<BnbAccWidth> &acc,
        const bnb_acc_elem_t<BnbAccWidth> &threshold
) { bsc_bnb_threshold<BnbAccWidth>(out, acc, threshold); }

template<size_t BnbAccWidth>
void inline bnb(
        bnb_acc_t<BnbAccWidth> &acc_out,
        const hv_t &a,
        const hv_t &b,
        const bnb_acc_t<BnbAccWidth> &acc_in
) { bsc_bnb(acc_out, a, b, acc_in); }

void inline dist(dist_t &out, const hv_t &a, const hv_t &b) { bsc_dist(out, a, b); }

template<size_t N>
void inline distN(
        dist_t (&dists)[N],
        const hv_t &query,
        const hv_t (&am)[N]
        ) {
    bsc_distN(dists, query, am);
}

template<size_t N>
void inline search(
        size_t &argmin,
        const hv_t &query,
        const hv_t (&am)[N]
        ) {
    bsc_search(argmin, query, am);
}
}
}

