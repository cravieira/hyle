#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <ap_int.h>

#include "bsc.hpp"
#include "common.hpp"
#include "defines.hpp"

const size_t VOICEHD_CLASSES = 26;
const size_t VOICEHD_FEATURES = 617;
const size_t VOICEHD_LEVELS = 10;
//using class_t = ap_uint<number_of_bits(VOICEHD_CLASSES)>;
using class_t = uint32_t;
using feat_t = uint32_t;

using feat_vec_t = std::array<feat_t, VOICEHD_FEATURES>;

//void voicehd_top(
//        class_t &pred,
//        const std::array<feat_t, VOICEHD_FEATURES> &features
//        );
//
//void voicehd_enc(
//        class_t &pred,
//        const feat_vec_t &features,
//        const std::array<hv_t, VOICEHD_FEATURES> &im,
//        const std::array<hv_t, VOICEHD_LEVELS>   &cim,
//        const std::array<hv_t, VOICEHD_CLASSES>  &am
//        );

//template<size_t N>
//using hv_mem_seg_t = std::array<std::array<hv_t, N>, HV_SEGMENTS>;
//void voicehd_enc_seg(
//        class_t &pred,
//        const feat_vec_t &features,
//        const hv_mem_seg_t<VOICEHD_FEATURES> &im,
//        const hv_mem_seg_t<VOICEHD_LEVELS>   &cim,
//        const hv_mem_seg_t<VOICEHD_CLASSES>  &am
//        );

// New version with plain C arrays
void voicehd_enc_seg(
        class_t &pred,
        const feat_vec_t &features,
        const hv_t (&im)[HV_SEGMENTS][VOICEHD_FEATURES],
        const hv_t (&cim)[HV_SEGMENTS][VOICEHD_LEVELS],
        const hv_t (&am)[HV_SEGMENTS][VOICEHD_CLASSES]
        );
