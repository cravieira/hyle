#include "bsc.hpp"
#include <cstddef>

void bsc_bind(hv_t &out, const hv_t &a, const hv_t &b) {
    out = a ^ b;
}

void bsc_bundle(hv_t &out, const hv_t &a, const hv_t &b, const hv_t &c) {
    //hls::vector<ap_uint<32>, 4> y = static_cast<ap_uint<32>>(10);
    constexpr size_t acc_bits = 2;
    using acc_elem_t = ap_uint<acc_bits>;
    using acc_t = hls::vector<acc_elem_t, DIM>;

    // Copy the LSB of each accumulator to the output
    MajColumn:
    for (size_t i = 0; i < DIM; i++) {
        acc_elem_t t_a = a[i];
        acc_elem_t t_b = b[i];
        acc_elem_t t_c = c[i];
        acc_elem_t sum = t_a + t_b + t_c;

        // Get the MSB of the sum. This should be equivalent to the thresholding
        out[i] = static_cast<bin_t>(sum[acc_bits-1]);
    }
}

