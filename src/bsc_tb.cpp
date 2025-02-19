#include <cstddef>
#include <iostream>

#include <ap_int.h>
#include <hls_vector.h>

#include "bsc.hpp"

using namespace vsa::bsc;

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

    bind(out, a, b);
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

    bindN(out, hvs);
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

    bundle(out, a, b, c);
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

    bundleN(out, hvs);
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
    bnb(acc, a, b, acc);
    bnb(acc, c, d, acc);
    bnb(acc, e, f, acc);
    bnb_threshold(out, acc, 1);

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

    dist(out, a, b);
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
    dist_t out_gold[2] = {7, 5};
    dist_t out[2];
    hv_t am[] = {a, b};

    distN(out, q, am);
    if (out[0] != out_gold[0] || out[1] != out_gold[1]) {
        std::cout << q << std::endl;
        std::cout << a << std::endl;
        std::cout << b << std::endl;
        std::cout << out[0] << ", " << out[1] << " != " << out_gold[0] << ", " << out_gold[1] << std::endl;
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

    search(out, q, am);
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

