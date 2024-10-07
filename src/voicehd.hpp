#include "common.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <ap_int.h>

const size_t VOICEHD_CLASSES = 26;
const size_t VOICEHD_FEATURES = 617;
const size_t VOICEHD_LEVELS = 10;
using class_t = ap_uint<number_of_bits(VOICEHD_CLASSES)>;
using feat_t = uint32_t;

void voicehd_top(
        class_t &pred,
        const std::array<feat_t, VOICEHD_FEATURES> &features
        );

void voicehd_enc(
        class_t &pred,
        const std::array<feat_t, VOICEHD_FEATURES> &features,
        const std::array<hv_t, VOICEHD_FEATURES> &im,
        const std::array<hv_t, VOICEHD_LEVELS>   &cim,
        const std::array<hv_t, VOICEHD_CLASSES>  &am
        );
