#include <cstddef>

#include "cgr.hpp"
#include "common.hpp"

constexpr size_t NUM_HVS = 32;

void cgr_bnb32(
        cgr_hv_t &out,
        const cgr_hv_t (&im1)[NUM_HVS],
        const cgr_hv_t (&im2)[NUM_HVS]
        ) {
    constexpr size_t BnbAccWidth = 3;
    cgr_bnb_acc_t<BnbAccWidth> bundle_acc;
    cgr_init_bnb_acc_t<BnbAccWidth>(bundle_acc);
    BnbLoop:
    for (int i = 0; i < NUM_HVS; i++) {
        cgr_bnb<BnbAccWidth>(bundle_acc, im1[i], im2[i], bundle_acc);
    }
    cgr_bnb_threshold<BnbAccWidth>(out, bundle_acc);
}

void cgr_bnb_threshold4(
        cgr_hv_t &out,
        const cgr_bnb_acc_t<4> &bundle_acc
        ) {
    #pragma HLS array_partition variable=bundle_acc dim=1

    cgr_bnb_threshold<4>(out, bundle_acc);
}
