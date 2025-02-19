#include "bsc.hpp"
#include <cstddef>

using namespace vsa::bsc;
namespace vsa {
namespace bsc {

#ifndef __SYNTHESIS__
// Non-member printer function for BSC hypervectors, i.e., hls::vector with
// binary digits
std::ostream& operator<<(std::ostream& os, const hv_t v) {
    for (auto it = v.cbegin(); it != v.cend(); it++) {
        //os << std::hex << std::setw(1) << std::setfill('0') << *it;
        os << *it;
    }

    return os;
}
#endif

void init_bnb_acc_t(bnb_acc_t &acc) { acc = static_cast<bnb_acc_elem_t>(0); }

void bind(hv_t &out, const hv_t &a, const hv_t &b) {
    out = a ^ b;
}

void bundle(hv_t &out, const hv_t &a, const hv_t &b, const hv_t &c) {
    //hls::vector<ap_uint<32>, 4> y = static_cast<ap_uint<32>>(10);
    constexpr size_t acc_bits = 2;
    using acc_elem_t = ap_uint<acc_bits>;

    // Copy the LSB of each accumulator to the output
    MajColumn:
    for (size_t i = 0; i < a.size(); i++) {
        acc_elem_t t_a = a[i];
        acc_elem_t t_b = b[i];
        acc_elem_t t_c = c[i];
        acc_elem_t sum = t_a + t_b + t_c;

        // Get the MSB of the sum. This should be equivalent to the thresholding
        out[i] = static_cast<hv_elem_t>(sum[acc_bits-1]);
    }
}

void bnb_threshold(
        hv_t &out,
        const bnb_acc_t &acc,
        const bnb_acc_elem_t &threshold) {
    BscBnbThreshold:
    for (size_t i = 0; i < acc.size(); i++) {
        hv_elem_t bit = acc[i] > threshold;
        out[i] = bit;
    }
}

void bnb(
        bnb_acc_t &acc_out,
        const hv_t &a,
        const hv_t &b,
        const bnb_acc_t &acc_in
        ) {
    hv_t temp = a^b;
    BscBnbAcc:
    for (size_t i = 0; i < temp.size(); i++) {
#pragma HLS unroll
        acc_out[i] = acc_in[i] + temp[i];
    }
}

void dist(dist_t &out, const hv_t &a, const hv_t &b) {
    hv_t temp = a^b;
    out = 0;

    AddReduce:
    for (size_t i = 0; i < a.size(); i++) {
        out = out + static_cast<dist_t>(temp[i]);
    }
}

}
}

