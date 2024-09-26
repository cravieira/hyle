#include <ap_int.h>
#include <hls_vector.h>
#include <array>
#include <cstddef>
#include <iostream>

constexpr int DIM = 10;
using bin_t = ap_uint<1>;
using hv_t = hls::vector<bin_t, DIM>;

// Utility functions
constexpr size_t number_of_bits(size_t x) {
    return x < 2 ? x : 1+number_of_bits(x >> 1);
}

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
    using acc_t = hls::vector<acc_elem_t, DIM>;

    // Copy the LSB of each accumulator to the output
    MajColumn:
    for (size_t col = 0; col < DIM; col++) {
        acc_elem_t sum = 0;

        AccColumn:
        for (size_t row = 0; row < N; row++) {
            sum += hvs[row][col];
        }

        // Get the MSB of the sum. This should be equivalent to the thresholding
        out[col] = static_cast<bin_t>(sum[acc_bits-1]);
    }
}

