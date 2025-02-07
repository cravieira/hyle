#include <cstddef>
#include <iostream>

#include <ap_int.h>
#include <hls_vector.h>

#include "bsc.hpp"

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
    hv_t hvs[] = {a, b, c};

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
    hv_t d =        {1, 0, 1, 0, 1, 1, 1, 1, 1, 1};
    hv_t e =        {0, 1, 0, 1, 1, 0, 0, 1, 1, 0};
    hv_t f =        {0, 1, 0, 1, 1, 0, 0, 1, 1, 0};
    hv_t g =        {1, 0, 1, 0, 1, 1, 1, 1, 1, 1};
    hv_t h =        {0, 1, 0, 1, 1, 0, 0, 1, 1, 0};
    hv_t i =        {0, 1, 0, 1, 1, 0, 0, 1, 1, 0};
    hv_t out_gold = {0, 1, 0, 1, 1, 0, 0, 1, 1, 0};
    hv_t out;
    hv_t hvs[] = {a, b, c, d, e, f, g, h, i};

    bsc_bundleN(out, hvs);
    if (out != out_gold) {
        std::cout << a << std::endl;
        std::cout << b << std::endl;
        std::cout << c << std::endl;
        std::cout << d << std::endl;
        std::cout << e << std::endl;
        std::cout << f << std::endl;
        std::cout << g << std::endl;
        std::cout << h << std::endl;
        std::cout << i << std::endl;
        std::cout << out << std::endl;
    };
}

void test_bnb() {
    hv_t a =        {1, 0, 0, 1, 1, 1, 0, 0, 1, 1};
    hv_t b =        {0, 1, 0, 1, 1, 0, 0, 1, 1, 0};
    hv_t c =        {0, 1, 1, 0, 1, 0, 0, 1, 1, 1};
    hv_t d =        {1, 0, 1, 0, 1, 0, 0, 0, 0, 1};
    hv_t e =        {0, 1, 0, 1, 1, 0, 0, 0, 0, 0};
    hv_t f =        {0, 1, 1, 1, 1, 0, 0, 1, 0, 1};
    hv_t out_gold = {1, 1, 0, 0, 0, 0, 0, 1, 0, 1};
    hv_t out;
    hv_t hvs[] = {a, b, c, d, e, f};

    bnb_acc_t acc = static_cast<bnb_acc_elem_t>(0);
    bsc_bnb(acc, a, b, acc);
    bsc_bnb(acc, c, d, acc);
    bsc_bnb(acc, e, f, acc);
    bsc_bnb_threshold(out, acc, 1);

    if (out != out_gold) {
        std::cout << a << std::endl;
        std::cout << b << std::endl;
        std::cout << c << std::endl;
        std::cout << d << std::endl;
        std::cout << e << std::endl;
        std::cout << f << std::endl;
        std::cout << out << std::endl;
    };
}


void test_dist() {
    hv_t a =        {1, 0, 1, 0, 1, 1, 1, 1, 1, 1};
    hv_t b =        {0, 1, 0, 1, 1, 0, 0, 1, 1, 0};
    dist_t out_gold = 7;
    dist_t out = -1;

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
    hls::vector<dist_t, 2> out_gold = {7, 5};
    hls::vector<dist_t, 2> out = static_cast<dist_t>(0);
    hv_t am[] = {a, b};

    bsc_distN(out, q, am);
    if (out != out_gold) {
        std::cout << q << std::endl;
        std::cout << a << std::endl;
        std::cout << b << std::endl;
        std::cout << out << " != " << out_gold << std::endl;
    };
}

void test_search() {
    hv_t q = {1, 0, 1, 0, 1, 1, 1, 1, 1, 1};
    hv_t a = {0, 1, 0, 1, 1, 0, 0, 1, 1, 0}; // Dist 7
    hv_t b = {1, 1, 1, 0, 1, 1, 0, 0, 0, 0}; // Dist 5
    hv_t c = {0, 1, 0, 1, 0, 0, 0, 0, 0, 0}; // Dist 10
    size_t out_gold = 1;
    size_t out = -1;
    hv_t am[] = {a, b, c};

    bsc_search(out, q, am);
    if (out != out_gold) {
        std::cout << q << std::endl;
        std::cout << a << std::endl;
        std::cout << b << std::endl;
        std::cout << c << std::endl;
        std::cout << out << " != " << out_gold << std::endl;
    };
}


int main () {
    //hv_t vec;
    //printf("Hello World\n");
    //std::cout << vec << std::endl;
    test_bind();
    test_bindN();
    test_bundleN();
    test_bnb();
    test_dist();
    test_distN();
    test_search();
}

