#pragma once

#include "hls_vector.h"
#include <cstddef>

/**
 * @brief Computes the number of bits in unsigned integer to store the passed
 * value
 *
 * @param x Value
 * @return Exact amount of bits to store the passed value
 */
constexpr size_t number_of_bits(size_t x) {
    return x < 2 ? x : 1+number_of_bits(x >> 1);
}

/**
 * @brief Check if value is power of 2
 *
 * @param v Value
 * @return 1 if power of 2, 0 otherwise.
 */
constexpr bool is_pow2(size_t v) {
//Snippet from: http://www.graphics.stanford.edu/~seander/bithacks.html#DetermineIfPowerOf2
    return v && !(v & (v - 1));
}

/**
 * @brief Parallel reset any matrix
 *
 * @tparam T Element data type
 * @tparam DIM1 Matrix dimension 1
 * @tparam DIM2 Matrix dimension 2
 * @param[inout] mat Matrix to be reset
 */
template<typename T, size_t DIM1, size_t DIM2>
void parallel_reset(T (&mat)[DIM1][DIM2]) {
    #pragma HLS inline

    for (size_t i = 0; i < DIM1; i++) {
        #pragma HLS unroll
        for (size_t j = 0; j < DIM2; j++) {
            #pragma HLS unroll
            mat[i][j] = static_cast<T>(0);
        }
    }
}
