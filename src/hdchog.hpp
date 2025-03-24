#include "vsa.hpp"
#include "bsc.hpp"
#include "cgr.hpp"
#include "dataset.hpp"
#include "defines.hpp"

constexpr int HOG_ORIENTATIONS = 9;
constexpr int HOG_MAGNITUDES = 11;
constexpr int HOG_CELLS = 36;
constexpr int HDCHOG_CLASSES = 10; // Classes in FashionMNIST

using orientation_t = feat_t;
using cell_t = orientation_t[HOG_ORIENTATIONS];
using hog_t = cell_t[HOG_CELLS];

using cell_mem_t  = hv_t [HV_SEGMENTS][HOG_CELLS];
using ori_mem_t = hv_t [HV_SEGMENTS][HOG_ORIENTATIONS];
using mag_mem_t = hv_t [HV_SEGMENTS][HOG_MAGNITUDES];
using am_t  = hv_t [HV_SEGMENTS][HDCHOG_CLASSES];

void hdchog(
        class_t &pred,
        const hog_t &input,
        const cell_mem_t (&cell_mem),
        const ori_mem_t (&ori_mem),
        const mag_mem_t (&mag_mem),
        const am_t (&am)
);
