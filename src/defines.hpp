#pragma once

#ifndef __HV_DIMENSIONS__
#define __HV_DIMENSIONS__ 1024
#endif

#ifndef __HV_SEGMENT_SIZE__
#define __HV_SEGMENT_SIZE__ __HV_DIMENSIONS__
#endif

#include <cstddef>

// Transform defines in constexpr constants. This allows for value assertion
// and constant definition at at compile time.
constexpr size_t DIM = __HV_DIMENSIONS__;
constexpr size_t HV_SEGMENT_SIZE = __HV_SEGMENT_SIZE__;
constexpr size_t HV_SEGMENTS = DIM/HV_SEGMENT_SIZE;

