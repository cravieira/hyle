#include <stdio.h>
#include <iostream>

#include <ap_int.h>
#include <hls_vector.h>

constexpr int DIM = 100;
using bin_t = ap_uint<1>;
using hv_t = hls::vector<bin_t, DIM>;

// Non-member printer function for hls::vector with binary digits
std::ostream& operator<<(std::ostream& os, const hv_t v) {
    for (auto it = v.cbegin(); it != v.cend(); it++) {
        //os << std::hex << std::setw(1) << std::setfill('0') << *it;
        os << *it;
    }

    return os;
}

int main () {
    hv_t vec;
    printf("Hello World\n");
    std::cout << vec << std::endl;
}


