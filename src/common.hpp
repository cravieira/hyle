#pragma once

#include <cstddef>

constexpr size_t number_of_bits(size_t x) {
    return x < 2 ? x : 1+number_of_bits(x >> 1);
}

