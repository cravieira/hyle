#include <cstddef>
#include <iostream>

#include <ap_int.h>
#include <hls_vector.h>

#include "cgr.hpp"
#include "common.hpp"

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
    hv_t a =        {1, 3, 1, 1, 3, 0, 2, 2, 1, 3};
    hv_t b =        {0, 1, 2, 1, 3, 0, 2, 1, 3, 2};
    hv_t out_gold = {1, 0, 3, 2, 2, 0, 0, 3, 0, 1};
    hv_t out;

    cgr_bind(out, a, b);
    if (out != out_gold) {
        std::cout << a << std::endl;
        std::cout << b << std::endl;
        std::cout << out << std::endl;
    };
}

void test_bindN() {
    hv_t a =        {0, 1, 1, 2, 2, 2, 3, 3, 1, 3};
    hv_t b =        {0, 0, 0, 0, 1, 2, 1, 0, 1, 3};
    hv_t c =        {0, 0, 1, 1, 2, 2, 1, 2, 1, 3};
    hv_t out_gold = {0, 1, 2, 3, 1, 2, 1, 1, 3, 1};
    hv_t out;
    hv_t hvs[] = {a, b, c};

    cgr_bindN(out, hvs);
    if (out != out_gold) {
        std::cout << a << std::endl;
        std::cout << b << std::endl;
        std::cout << c << std::endl;
        std::cout << out << std::endl;
    };
}

void test_bundle() {
    // Bundling-ties should be solved deterministacally by picking the lowest
    // value among the ties. E.g., the bundle of 3+2+1 is equal to "1", as it
    // is the lowest value among the ties.
    hv_t a =        {1, 0, 1, 3, 0, 1, 3, 3, 2, 3};
    hv_t b =        {0, 1, 2, 3, 1, 1, 1, 2, 2, 1};
    hv_t c =        {0, 1, 2, 3, 2, 2, 2, 1, 2, 3};
    hv_t out_gold = {0, 1, 2, 3, 0, 1, 1, 1, 2, 3};
    hv_t out;

    cgr_bundle(out, a, b, c);
    if (out != out_gold) {
        std::cout << a << std::endl;
        std::cout << b << std::endl;
        std::cout << c << std::endl;
        std::cout << out << std::endl;
    };
}

void test_bundleN() {
    hv_t a =        {3, 1, 0, 2, 2, 2, 2, 3, 2, 1};
    hv_t b =        {0, 1, 3, 3, 3, 0, 2, 3, 1, 1};
    hv_t c =        {0, 1, 0, 1, 2, 3, 2, 3, 2, 1};
    hv_t d =        {3, 2, 1, 1, 1, 3, 2, 3, 1, 1};
    hv_t e =        {2, 2, 3, 2, 0, 3, 2, 3, 1, 1};
    hv_t f =        {1, 2, 3, 3, 1, 3, 2, 3, 2, 1};
    hv_t g =        {3, 2, 0, 0, 1, 1, 1, 3, 2, 1};
    hv_t h =        {1, 2, 0, 1, 0, 1, 1, 3, 1, 1};
    hv_t out_gold = {3, 2, 0, 1, 1, 3, 2, 3, 1, 1};
    hv_t out;
    hv_t hvs[] = {a, b, c, d, e, f, g, h};

    cgr_bundleN(out, hvs);
    if (out != out_gold) {
        std::cout << a << std::endl;
        std::cout << b << std::endl;
        std::cout << c << std::endl;
        std::cout << d << std::endl;
        std::cout << e << std::endl;
        std::cout << f << std::endl;
        std::cout << g << std::endl;
        std::cout << h << std::endl;
        std::cout << out << std::endl;
    };
}

void test_bnb() {
    hv_t a =        {1, 1, 2, 3, 1, 0, 1, 3, 1, 1};
    hv_t b =        {2, 0, 3, 3, 2, 0, 1, 3, 2, 3};
    hv_t c =        {3, 0, 1, 2, 0, 0, 1, 3, 1, 2};
    hv_t d =        {0, 0, 1, 2, 3, 0, 1, 3, 1, 2};
    hv_t e =        {1, 2, 0, 1, 0, 0, 1, 3, 1, 3};
    hv_t f =        {2, 1, 1, 1, 0, 0, 1, 3, 0, 1};
    hv_t out_gold = {3, 0, 1, 2, 3, 0, 2, 2, 1, 0};
    hv_t out;
    hv_t hvs[] = {a, b, c, d, e, f};

    bnb_acc_t acc;
    init_bnb_acc_t(acc);

    cgr_bnb(acc, a, b, acc);
    cgr_bnb(acc, c, d, acc);
    cgr_bnb(acc, e, f, acc);
    cgr_bnb_threshold(out, acc);

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
    hv_t a =        {1, 0, 1, 3, 0, 3, 2, 1, 3, 1};
    hv_t b =        {0, 1, 2, 1, 3, 0, 2, 3, 1, 1};
    dist_t out_gold = 11;
    dist_t out = -1;

    cgr_dist(out, a, b);
    if (out != out_gold) {
        std::cout << a << std::endl;
        std::cout << b << std::endl;
        std::cout << out << " != " << out_gold << std::endl;
    };
}

void test_distN() {
    hv_t q = {2, 1, 3, 0, 2, 1, 3, 0, 2, 2};
    hv_t a = {0, 1, 2, 3, 0, 1, 2, 3, 0, 1};
    hv_t b = {2, 3, 0, 1, 1, 2, 3, 2, 2, 0};
    hls::vector<dist_t, 2> out_gold = {11, 10};
    hls::vector<dist_t, 2> out = static_cast<dist_t>(0);
    hv_t am[] = {a, b};

    cgr_distN(out, q, am);
    if (out != out_gold) {
        std::cout << q << std::endl;
        std::cout << a << std::endl;
        std::cout << b << std::endl;
        std::cout << out << " != " << out_gold << std::endl;
    };
}

void test_search() {
    hv_t q = {2, 1, 3, 0, 2, 1, 3, 0, 2, 2};
    hv_t a = {0, 1, 2, 3, 0, 1, 2, 3, 0, 1}; // Dist 11
    hv_t b = {2, 3, 0, 1, 1, 2, 3, 2, 2, 0}; // Dist 10
    hv_t c = {0, 3, 1, 2, 0, 3, 1, 2, 0, 0}; // Dist 20
    size_t out_gold = 1;
    size_t out = -1;
    hv_t am[] = {a, b, c};

    cgr_search(out, q, am);
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
    //std::cout << vec << std::endl;
    printf("Running CGR tests...\n");
    test_bind();
    test_bindN();
    test_bundle();
    test_bundleN();
    test_bnb();
    test_dist();
    test_distN();
    test_search();
}

