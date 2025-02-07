#pragma once

#ifndef __HV_DIMENSIONS__
#define __HV_DIMENSIONS__ 1024
#endif

#ifndef __HV_SEGMENT_SIZE__
#define __HV_SEGMENT_SIZE__ __HV_DIMENSIONS__
#endif

// Number of parallel datapaths to process each HV segment
#ifndef __SEGMENT_DATAPATHS__
#define __SEGMENT_DATAPATHS__ 1
#endif

#include <cstddef>

// Transform defines in constexpr constants. This allows for value assertion
// and constant definition at at compile time.
constexpr size_t DIM = __HV_DIMENSIONS__; // TODO: Maybe rename "DIM" to "HV_DIM" as it is more meaningful
constexpr size_t HV_SEGMENT_SIZE = __HV_SEGMENT_SIZE__;
constexpr size_t HV_SEGMENTS = DIM/HV_SEGMENT_SIZE;
constexpr size_t SEGMENT_DATAPATHS = __SEGMENT_DATAPATHS__;

