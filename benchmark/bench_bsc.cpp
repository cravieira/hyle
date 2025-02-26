#include <cstddef>

#include "bsc.hpp"
#include "common.hpp"

constexpr size_t NUM_HVS = 32;

using namespace vsa::bsc;

void bsc_bnb_32(
        hv_t &out,
        const hv_t (&im1)[NUM_HVS],
        const hv_t (&im2)[NUM_HVS]
        ) {
    bnb_acc_t bundle_acc = static_cast<bnb_acc_elem_t>(0);
    BnbLoop:
    for (int i = 0; i < NUM_HVS; i++) {
        bsc_bnb(bundle_acc, im1[i], im2[i], bundle_acc);
    }
    bsc_bnb_threshold(out, bundle_acc, NUM_HVS/2);
}

