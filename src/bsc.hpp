#pragma once

#include <ap_int.h>
#include <hls_vector.h>
#include <cstddef>
#include <iostream>

#include "common.hpp"
#include "defines.hpp"

namespace vsa {
namespace bsc {

// Type definitions
using hv_elem_t = ap_uint<1>;
using hv_t = hls::vector<hv_elem_t, HV_SEGMENT_SIZE>;
// An uint big enough to store the number of dimensions
using dim_t = ap_uint<number_of_bits(HV_SEGMENT_SIZE*HV_SEGMENTS)>;
// An uint big enought to store the max distance between BSC HVs.
using dist_t = ap_uint<number_of_bits(DIM)>;

// Special types used for the "Bind and Bundle" operator
using bnb_acc_elem_t = ap_uint<10>; // TODO: This can later be optimized to fit
                                    // only the necessary bits in accumulation.
using bnb_acc_t = hls::vector<bnb_acc_elem_t, HV_SEGMENT_SIZE>;

void init_bnb_acc_t(bnb_acc_t &acc);

// Non-member printer function for BSC hypervectors, i.e., hls::vector with
// binary digits
std::ostream& operator<<(std::ostream& os, const hv_t v);

// BSC

void bind(hv_t &out, const hv_t &a, const hv_t &b);

template<size_t N>
void bindN(hv_t &out, const hv_t (&hvs)[N]) {
    static_assert(
            N > 2,
            "Number of vectors in \"hvs\" collection must be greater than 2");

    out = hvs[0] ^ hvs[1];
    BindLoop:
    for (size_t i = 2; i < N; i++) {
        out ^= hvs[i];
    }
}

void bundle(hv_t &out, const hv_t &a, const hv_t &b, const hv_t &c);

template<size_t N>
void bundleN(hv_t &out, const hv_t (&hvs)[N]) {
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
        hv_elem_t bit = sum > threshold ? 1 : 0;
        out[col] = bit;
    }
}

void bnb_threshold(
        hv_t &out,
        const bnb_acc_t &acc,
        const bnb_acc_elem_t &threshold);

void bnb(
        bnb_acc_t &acc_out,
        const hv_t &a,
        const hv_t &b,
        const bnb_acc_t &acc_in
);

void dist(dist_t &out, const hv_t &a, const hv_t &b);

template<size_t N>
void distN(
        hls::vector<dist_t, N> &dists,
        const hv_t &query,
        const hv_t (&am)[N]
        ) {
    // TODO: Using a sub function might not be the best for the HLS tool to create
    // an optimized design, but it is the easiest implementation at the moment.
    // Revisit this function implementation when optimizing code.

    ComputeDist:
    for (size_t i = 0; i < N; i++) {
        dist(dists[i], query, am[i]);
    }
}

template<size_t N>
void search(
        size_t &argmin,
        const hv_t &query,
        const hv_t (&am)[N]
        ) {
    static_assert(N > 0, "The size of the associative memory must be greater than 1");
    // TODO: Using a sub function might not be the best for the HLS tool to create
    // an optimized design, but it is the easiest implementation at the moment.
    // Revisit this function implementation when optimizing code.

    hls::vector<dist_t, N> dists = static_cast<dist_t>(0); // TODO: Should this buffer be static?
    distN(dists, query, am);

    //using ind_t = ap_uint<number_of_bits(N)>;
    argmin = 0;
    dist_t val = dists[0];
    Argmin:
    for (size_t i = 1; i < N; i++) {
        if (val > dists[i]) {
            val = dists[i];
            argmin = i;
        }
    }
}

}
}

