#include <cstddef>

#include "bsc.hpp"
#include "common.hpp"

#ifndef __BNB_ACC_WIDTH__
#define __BNB_ACC_WIDTH__ -1
#endif
constexpr size_t BnbAccWidth = __BNB_ACC_WIDTH__;

constexpr size_t NUM_HVS = 32;

void bsc_bnb32(
        bsc_hv_t &out,
        const bsc_hv_t (&im1)[NUM_HVS],
        const bsc_hv_t (&im2)[NUM_HVS]
        ) {
    bsc_bnb_acc_t<BnbAccWidth> bundle_acc;
    bsc_init_bnb_acc_t<BnbAccWidth>(bundle_acc);
    BnbLoop:
    for (int i = 0; i < NUM_HVS; i++) {
        bsc_bnb<BnbAccWidth>(bundle_acc, im1[i], im2[i], bundle_acc);
    }
    bsc_bnb_threshold<BnbAccWidth>(out, bundle_acc, NUM_HVS/2);
}

void bsc_bnb_threshold4(
        bsc_hv_t &out,
        const bsc_bnb_acc_t<4> &bundle_acc
        ) {
    #pragma HLS array_partition variable=bundle_acc dim=1

    bsc_bnb_acc_elem_t<4> threshold = 7;
    bsc_bnb_threshold<4>(out, bundle_acc, threshold);
}
