#include "cgr.hpp"

#include <cstdint>
#include <hls_vector.h>

// Since every type is aliased in cgr.hpp, with the line below it is possible to
// use types defined in "vsa::cgr" instead of the long non-namespaced type name.
// Thus, "hv_t" can be used instead of "cgr_hv_t".
using namespace vsa::cgr;

#ifndef __SYNTHESIS__
// Non-member printer function for CGR hypervectors, i.e., hls::vector with
// uint digits
std::ostream& operator<<(std::ostream& os, const hv_t v) {
    for (auto it = v.cbegin(); it != v.cend(); it++) {
        //os << std::hex << std::setw(1) << std::setfill('0') << *it;
        os << *it;
    }

    return os;
}
#endif

void cgr_bind(hv_t &out, const hv_t &a, const hv_t &b) {
    out = a + b;
}

void cgr_bundle(hv_t &out, const hv_t &a, const hv_t &b, const hv_t &c) {
    constexpr size_t acc_bits = 2;
    using acc_elem_t = ap_uint<acc_bits>;
    using acc_bank_t = hls::vector<acc_elem_t, __CGR_POINTS__>;

    bnb_acc_t<2> acc_bank_vec;
    hv_t hvs[3] = {a, b, c};

    init_bnb_acc_t<2>(acc_bank_vec);

    CgrBundleVec:
    for (int v = 0; v < 3; v++) { // TODO: Maybe pipeline or unroll this loop
        CgrBundleVecElem:
        for (int e = 0; e < a.size(); e++) {
            #pragma HLS unroll
            acc_bank_vec[e][hvs[v][e]]++;
        }
    }

    // Threshold to compute the bundled HV
    Threshold:
    for (int i = 0; i < BNB_WIDTH; i++) {
        #pragma HLS unroll
        // TODO: Maybe size_t could be optimized to return the exact amount of bits only
        size_t argmax;
        parallel_argmax(argmax, acc_bank_vec[i]);
        out[i] = argmax;
    }
}

void cgr_dist(dist_t &out, const hv_t &a, const hv_t &b) {
    hv_t t1 = a-b;
    hv_t t2 = b-a;
    out = 0;

    AddReduce:
    for (size_t i = 0; i < a.size(); i++) {
        hv_elem_t min = t1[i] < t2[i] ? t1[i] : t2[i];
        out = out + static_cast<dist_t>(min);
    }
}

