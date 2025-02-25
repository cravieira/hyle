#include <cstddef>

#include "cgr.hpp"
#include "common.hpp"

constexpr size_t NUM_HVS = 32;

void cgr_bnb32(
        cgr_hv_t &out,
        const cgr_hv_t (&im1)[NUM_HVS],
        const cgr_hv_t (&im2)[NUM_HVS]
        ) {
    cgr_bnb_acc_t bundle_acc;
    cgr_init_bnb_acc_t(bundle_acc);
    BnbLoop:
    for (int i = 0; i < NUM_HVS; i++) {
        cgr_bnb(bundle_acc, im1[i], im2[i], bundle_acc);
    }
    cgr_bnb_threshold(out, bundle_acc);
}

