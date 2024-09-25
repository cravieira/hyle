#include <iostream>

#include <ap_int.h>
#include <hls_vector.h>

#include "bsc.hpp"

// Non-member printer function for hls::vector with binary digits
std::ostream& operator<<(std::ostream& os, const hv_t v) {
    for (auto it = v.cbegin(); it != v.cend(); it++) {
        //os << std::hex << std::setw(1) << std::setfill('0') << *it;
        os << *it;
    }

    return os;
}

void test_bind() {
    hv_t a =        {1, 0, 1, 0, 1, 1, 1, 1, 1, 1};
    hv_t b =        {0, 1, 0, 1, 1, 0, 0, 1, 1, 0};
    hv_t out_gold = {1, 1, 1, 1, 0, 1, 1, 0, 0, 1};
    hv_t out;

    bsc_bind(out, a, b);
}

void test_bundle() {
    hv_t a =        {1, 0, 1, 0, 1, 1, 1, 1, 1, 1};
    hv_t b =        {0, 1, 0, 1, 1, 0, 0, 1, 1, 0};
    hv_t c =        {0, 1, 0, 1, 1, 0, 0, 1, 1, 0};
    hv_t out_gold = {0, 1, 0, 1, 1, 0, 0, 1, 1, 0};
    hv_t out;

    bsc_bundle(out, a, b, c);
    if (out == out_gold) {
        std::cout << a << std::endl;
        std::cout << b << std::endl;
        std::cout << c << std::endl;
        std::cout << out << std::endl;
    };
}

int main () {
    //hv_t vec;
    //printf("Hello World\n");
    //std::cout << vec << std::endl;
    //test_bind();
    test_bundle();
}


