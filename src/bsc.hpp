#pragma once

#include <ap_int.h>
#include <hls_vector.h>
#include <array>
#include <cstddef>
#include <iostream>

#include "common.hpp"

// Type definitions
constexpr int DIM = 10;
using bin_t = ap_uint<1>;
using hv_t = hls::vector<bin_t, DIM>;
// An uint big enough to store the number of dimensions
using dim_t = ap_uint<number_of_bits(DIM)>;

// Non-member printer function for BSC hypervectors, i.e., hls::vector with
// binary digits
std::ostream& operator<<(std::ostream& os, const hv_t v);

// BSC

void bsc_bind(hv_t &out, const hv_t &a, const hv_t &b);

template<size_t N>
void bsc_bindN(hv_t &out, const std::array<hv_t, N> &hvs) {
    static_assert(
            N > 2,
            "Number of vectors in \"hvs\" collection must be greater than 2");

    out = hvs[0] ^ hvs[1];
    BindLoop:
    for (size_t i = 2; i < N; i++) {
        out ^= hvs[i];
    }
}

void bsc_bundle(hv_t &out, const hv_t &a, const hv_t &b, const hv_t &c);

template<size_t N>
void bsc_bundleN(hv_t &out, const std::array<hv_t, N> &hvs) {
    constexpr size_t acc_bits = number_of_bits(N);
    using acc_elem_t = ap_uint<acc_bits>;

    // Copy the LSB of each accumulator to the output
    MajColumn:
    for (size_t col = 0; col < DIM; col++) {
        acc_elem_t sum = 0;

        AccColumn:
        for (size_t row = 0; row < N; row++) {
            sum += hvs[row][col];
        }

        const acc_elem_t threshold(N/2);
        // The use of ">" instead of ">=" makes the results equal to TorchHD
        bin_t bit = sum > threshold ? 1 : 0;
        out[col] = bit;
    }
}

void bsc_dist(dim_t &out, const hv_t &a, const hv_t &b);

template<size_t N>
void bsc_distN(
        hls::vector<dim_t, N> &dists,
        const hv_t &query,
        const std::array<hv_t, N> am) {
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
        const hv_t &query,
        const std::array<hv_t, N> &am) {
    static_assert(N > 0, "The size of the associative memory must be greater than 1");
    // TODO: Using a sub function might not be the best for the HLS tool to create
    // an optimized design, but it is the easiest implementation at the moment.
    // Revisit this function implementation when optimizing code.

    hls::vector<dim_t, N> dists = static_cast<dim_t>(0); // TODO: Should this buffer be static?
    bsc_distN(dists, query, am);

    //using ind_t = ap_uint<number_of_bits(N)>;
    argmin = 0;
    dim_t val = dists[0];
    Argmin:
    for (size_t i = 1; i < N; i++) {
        if (val > dists[i]) {
            val = dists[i];
            argmin = i;
        }
    }
}
