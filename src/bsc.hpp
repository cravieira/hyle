#include <ap_int.h>
#include <hls_vector.h>
#include <array>
#include <cstddef>
#include <iostream>

constexpr int DIM = 10;
using bin_t = ap_uint<1>;
using hv_t = hls::vector<bin_t, DIM>;

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

