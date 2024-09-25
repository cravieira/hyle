#include <ap_int.h>
#include <hls_vector.h>

constexpr int DIM = 10;
using bin_t = ap_uint<1>;
using hv_t = hls::vector<bin_t, DIM>;

void bsc_bind(hv_t &out, const hv_t &a, const hv_t &b);
void bsc_bundle(hv_t &out, const hv_t &a, const hv_t &b, const hv_t &c);

