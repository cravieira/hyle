#pragma once

#include <ap_int.h>
#include <hls_vector.h>
#include <cstddef>
#include <iostream>

#include "common.hpp"
#include "defines.hpp"

// Number of points used in the quantized circle
#ifndef __CGR_POINTS__
#define __CGR_POINTS__ 4
#endif

// Ensure the number of points is always a power of 2 to avoid modular checking
// hardware.
static_assert(
        is_pow2(__CGR_POINTS__),
        "Define __CGR_POINTS__ is not a power of 2!"
        );

constexpr size_t _cgr_bits_per_dim = number_of_bits(__CGR_POINTS__-1);
using cgr_hv_elem_t = ap_uint<number_of_bits(_cgr_bits_per_dim)>;
using cgr_hv_t = hls::vector<cgr_hv_elem_t, HV_SEGMENT_SIZE>;
// An uint big enough to store the number of dimensions used in the application.
using cgr_dim_t = ap_uint<number_of_bits(DIM)>;
// An uint big enough to store the maximal distance between CGR HVs. The maximal
// distance between CGR elements is #CGR_POINTS/2.
using cgr_dist_t = ap_uint<number_of_bits(DIM*(__CGR_POINTS__/2))>;

// Special types used for the "Bind and Bundle" operator
// TODO: This can later be optimized to fit only the necessary bits in accumulation.
using cgr_bnb_acc_elem_t = ap_uint<10>;
constexpr size_t BNB_WIDTH = HV_SEGMENT_SIZE;
constexpr size_t BNB_HEIGHT = __CGR_POINTS__;
using cgr_bnb_acc_t = cgr_bnb_acc_elem_t[BNB_WIDTH][BNB_HEIGHT];

// Non-member printer function for CGR hypervectors, i.e., hls::vector with
// binary digits
std::ostream& operator<<(std::ostream& os, const cgr_hv_t v);

// Free function alternatives
void cgr_init_bnb_acc_t(cgr_bnb_acc_t &acc);

void cgr_bind(cgr_hv_t &out, const cgr_hv_t &a, const cgr_hv_t &b);

template<size_t N>
void inline cgr_bindN(cgr_hv_t &out, const cgr_hv_t (&hvs)[N]) {
    static_assert(
            N > 2,
            "Number of vectors in \"hvs\" collection must be greater than 2");

    out = hvs[0] + hvs[1];
    BindLoop:
    for (size_t i = 2; i < N; i++) {
        out += hvs[i];
    }
}

void cgr_bundle(cgr_hv_t &out, const cgr_hv_t &a, const cgr_hv_t &b, const cgr_hv_t &c);

template<size_t N>
void inline cgr_bundleN(cgr_hv_t &out, const cgr_hv_t (&hvs)[N]) {
    constexpr size_t acc_bits = number_of_bits(N);
    using acc_elem_t = ap_uint<acc_bits>;
    using acc_bank_t = hls::vector<acc_elem_t, __CGR_POINTS__>;

    cgr_bnb_acc_t acc_bank_vec;
    cgr_init_bnb_acc_t(acc_bank_vec);

    CgrBundleVec:
    for (int v = 0; v < N; v++) { // TODO: Maybe pipeline or unroll this loop
        CgrBundleVecElem:
        for (int e = 0; e < HV_SEGMENT_SIZE; e++) {
            #pragma HLS unroll
            acc_bank_vec[e][hvs[v][e]]++;
        }
    }

    // Threshold to compute the bundled HV
    for (int i = 0; i < BNB_WIDTH; i++) {
        #pragma HLS unroll
        // TODO: Maybe size_t could be optimized to return the exact amount of bits only
        size_t argmax;
        parallel_argmax(argmax, acc_bank_vec[i]);
        out[i] = argmax;
    }
}

void cgr_bnb_threshold(cgr_hv_t &out, const cgr_bnb_acc_t &acc);

void cgr_bnb(cgr_bnb_acc_t &acc_out, const cgr_hv_t &a, const cgr_hv_t &b, const cgr_bnb_acc_t &acc_in);

void cgr_dist(cgr_dist_t &out, const cgr_hv_t &a, const cgr_hv_t &b);

template<size_t N>
void cgr_distN(
        cgr_dist_t (&dists)[N],
        const cgr_hv_t &query,
        const cgr_hv_t (&am)[N]
        ) {
    ComputeDist:
    for (size_t i = 0; i < N; i++) {
        cgr_dist(dists[i], query, am[i]);
    }
}

template<size_t N>
void cgr_search(
        size_t &argmin,
        const cgr_hv_t &query,
        const cgr_hv_t (&am)[N]
        ) {
    static_assert(N > 0, "The size of the associative memory must be greater than 1");
    // TODO: Using a sub function might not be the best for the HLS tool to create
    // an optimized design, but it is the easiest implementation at the moment.
    // Revisit this function implementation when optimizing code.

    cgr_dist_t dists[N];
    cgr_distN(dists, query, am);

    argmin = 0;
    cgr_dist_t val = dists[0];
    Argmin:
    for (size_t i = 1; i < N; i++) {
        if (val > dists[i]) {
            val = dists[i];
            argmin = i;
        }
    }
}

// Namespace member functions
namespace vsa {
namespace cgr {

using hv_elem_t = cgr_hv_elem_t;
using hv_t = cgr_hv_t;
using dim_t = cgr_dim_t;
using dist_t = cgr_dist_t;

// Special types used for the "Bind and Bundle" operator
// TODO: This can later be optimized to fit only the necessary bits in accumulation.
using bnb_acc_elem_t = cgr_bnb_acc_elem_t;
using bnb_acc_t = cgr_bnb_acc_t;

void inline init_bnb_acc_t(bnb_acc_t &acc) { cgr_init_bnb_acc_t(acc); }

// TODO: The types defined in bundling functions and the bnb types are very
// similar. Maybe they could be merged to simplify the code.

// CGR
void inline bind(hv_t &out, const hv_t &a, const hv_t &b) { cgr_bind(out, a, b); }

template<size_t N>
void inline bindN(hv_t &out, const hv_t (&hvs)[N]) { cgr_bindN(out, hvs); }

void inline bundle(hv_t &out, const hv_t &a, const hv_t &b, const hv_t &c) {cgr_bundle(out, a, b, c); }

template<size_t N>
void inline bundleN(hv_t &out, const hv_t (&hvs)[N]) { cgr_bundleN(out, hvs); }

void inline bnb_threshold(
        hv_t &out,
        const bnb_acc_t &acc
) { cgr_bnb_threshold(out, acc); }

void inline bnb(
        bnb_acc_t &acc_out,
        const hv_t &a,
        const hv_t &b,
        const bnb_acc_t &acc_in
) { cgr_bnb(acc_out, a, b, acc_in); }

void inline dist(dist_t &out, const hv_t &a, const hv_t &b) { cgr_dist(out, a, b); }

template<size_t N>
void inline distN(
        dist_t (&dists)[N],
        const hv_t &query,
        const hv_t (&am)[N]
        ) {
    cgr_distN(dists, query, am);
}

template<size_t N>
void inline search(
        size_t &argmin,
        const hv_t &query,
        const hv_t (&am)[N]
        ) {
    cgr_search(argmin, query, am);
}

}
}
