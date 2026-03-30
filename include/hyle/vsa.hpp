#pragma once

#include "bsc.hpp"
#include "cgr.hpp"

// Choose the standard namespace in use according to the HDC class chosen
#if __VSA_BSC__
    using namespace vsa::bsc;
#elif __VSA_CGR__
    using namespace vsa::cgr;
#else
    #error "Define a VSA class"
#endif

