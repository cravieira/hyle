#include <cstddef>

#include "bsc.hpp"
#include "common.hpp"

constexpr size_t NUM_HVS = 32;

void bsc_bnb32(
        bsc_hv_t &out,
        const bsc_hv_t (&im1)[NUM_HVS],
        const bsc_hv_t (&im2)[NUM_HVS]
        ) {
    bsc_bnb_acc_t bundle_acc;
    bsc_init_bnb_acc_t(bundle_acc);
    BnbLoop:
    for (int i = 0; i < NUM_HVS; i++) {
        bsc_bnb(bundle_acc, im1[i], im2[i], bundle_acc);
    }
    bsc_bnb_threshold(out, bundle_acc, NUM_HVS/2);
}

