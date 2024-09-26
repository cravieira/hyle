#include "bsc.hpp"
#include <cstddef>

// Non-member printer function for BSC hypervectors, i.e., hls::vector with
// binary digits
std::ostream& operator<<(std::ostream& os, const hv_t v) {
    for (auto it = v.cbegin(); it != v.cend(); it++) {
        //os << std::hex << std::setw(1) << std::setfill('0') << *it;
        os << *it;
    }

    return os;
}

void bsc_bind(hv_t &out, const hv_t &a, const hv_t &b) {
    out = a ^ b;
}

void bsc_bundle(hv_t &out, const hv_t &a, const hv_t &b, const hv_t &c) {
    //hls::vector<ap_uint<32>, 4> y = static_cast<ap_uint<32>>(10);
    constexpr size_t acc_bits = 2;
    using acc_elem_t = ap_uint<acc_bits>;

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

void bsc_dist(dim_t &out, const hv_t &a, const hv_t &b) {
    hv_t temp = a^b;
    out = 0;

    AddReduce:
    for (size_t i = 0; i < DIM; i++) {
        out = out + static_cast<dim_t>(temp[i]);
    }
}
