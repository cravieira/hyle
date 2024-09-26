#include <array>
#include <cstddef>
#include <iostream>

#include <ap_int.h>
#include <hls_vector.h>

#include "bsc.hpp"

// Non-member printer function for BSC hypervectors, i.e., hls::vector with
// binary digits
std::ostream& operator<<(std::ostream& os, const hv_t v) {
    for (auto it = v.cbegin(); it != v.cend(); it++) {
        //os << std::hex << std::setw(1) << std::setfill('0') << *it;
        os << *it;
    }

    return os;
}

// Pretty printer for hls::vector
template<typename T, size_t N>
std::ostream& operator<<(std::ostream& os, const hls::vector<T, N> c) {
    for (auto it = c.cbegin(); it != c.cend(); it++) {
        //os << std::hex << std::setw(1) << std::setfill('0') << *it;
        os << *it << " ";
    }

    return os;
}

void test_bind() {
    hv_t a =        {1, 0, 1, 0, 1, 1, 1, 1, 1, 1};
    hv_t b =        {0, 1, 0, 1, 1, 0, 0, 1, 1, 0};
    hv_t out_gold = {1, 1, 1, 1, 0, 1, 1, 0, 0, 1};
    hv_t out;

    bsc_bind(out, a, b);
    if (out != out_gold) {
        std::cout << a << std::endl;
        std::cout << b << std::endl;
        std::cout << out << std::endl;
    };
}

void test_bindN() {
    hv_t a =        {1, 0, 1, 0, 1, 1, 1, 1, 1, 0};
    hv_t b =        {0, 1, 0, 1, 1, 0, 0, 1, 1, 0};
    hv_t c =        {1, 0, 0, 1, 1, 0, 0, 1, 1, 0};
    hv_t out_gold = {0, 1, 1, 0, 1, 1, 1, 1, 1, 0};
    hv_t out;
    std::array<hv_t, 3> hvs = {a, b, c};

    bsc_bindN(out, hvs);
    if (out != out_gold) {
        std::cout << a << std::endl;
        std::cout << b << std::endl;
        std::cout << c << std::endl;
        std::cout << out << std::endl;
    };
}

void test_bundle() {
    hv_t a =        {1, 0, 1, 0, 1, 1, 1, 1, 1, 1};
    hv_t b =        {0, 1, 0, 1, 1, 0, 0, 1, 1, 0};
    hv_t c =        {0, 1, 0, 1, 1, 0, 0, 1, 1, 0};
    hv_t out_gold = {0, 1, 0, 1, 1, 0, 0, 1, 1, 0};
    hv_t out;

    bsc_bundle(out, a, b, c);
    if (out != out_gold) {
        std::cout << a << std::endl;
        std::cout << b << std::endl;
        std::cout << c << std::endl;
        std::cout << out << std::endl;
    };
}

void test_bundleN() {
    hv_t a =        {1, 0, 1, 0, 1, 1, 1, 1, 1, 1};
    hv_t b =        {0, 1, 0, 1, 1, 0, 0, 1, 1, 0};
    hv_t c =        {0, 1, 0, 1, 1, 0, 0, 1, 1, 0};
    hv_t out_gold = {0, 1, 0, 1, 1, 0, 0, 1, 1, 0};
    hv_t out;
    std::array<hv_t, 3> hvs = {a, b, c};

    bsc_bundleN(out, hvs);
    if (out != out_gold) {
        std::cout << a << std::endl;
        std::cout << b << std::endl;
        std::cout << c << std::endl;
        std::cout << out << std::endl;
    };
}

void test_dist() {
    hv_t a =        {1, 0, 1, 0, 1, 1, 1, 1, 1, 1};
    hv_t b =        {0, 1, 0, 1, 1, 0, 0, 1, 1, 0};
    dim_t out_gold = 7;
    dim_t out = -1;

    bsc_dist(out, a, b);
    if (out != out_gold) {
        std::cout << a << std::endl;
        std::cout << b << std::endl;
        std::cout << out << " != " << out_gold << std::endl;
    };
}

void test_distN() {
    hv_t q =        {1, 0, 1, 0, 1, 1, 1, 1, 1, 1};
    hv_t a =        {0, 1, 0, 1, 1, 0, 0, 1, 1, 0};
    hv_t b =        {1, 1, 1, 0, 1, 1, 0, 0, 0, 0};
    hls::vector<dim_t, 2> out_gold = {7, 5};
    hls::vector<dim_t, 2> out = static_cast<dim_t>(0);
    std::array<hv_t, 2> am = {a, b};

    bsc_distN(out, q, am);
    if (out != out_gold) {
        std::cout << q << std::endl;
        std::cout << a << std::endl;
        std::cout << b << std::endl;
        std::cout << out << " != " << out_gold << std::endl;
    };
}

int main () {
    //hv_t vec;
    //printf("Hello World\n");
    //std::cout << vec << std::endl;
    //test_bind();
    //test_bindN();
    //test_bundleN();
    //test_dist();
    test_distN();
}

