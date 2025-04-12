#pragma once

#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

#include "vsa.hpp" // Define standard hv_t used in simulation

using class_t = uint8_t;
//using feat_t = uint8_t;
typedef std::vector<uint32_t> data_t;
typedef std::vector<data_t> dataset_t;
typedef std::vector<class_t> label_t;

#ifndef __SYNTHESIS__
dataset_t read_dataset(const std::string& path);
label_t read_labels(const std::string& path);

// HDC Memory loaders //
// HV memory type. Types used for memory file handling
using mem_data_t = std::vector<std::vector<hv_elem_t>>;

mem_data_t parse_mem_file(const std::string &path);

template<size_t N>
void segment_mem(const mem_data_t &parsed_mem, hv_t (&reshaped_mem)[HV_SEGMENTS][N]) {
    const size_t expected_hv_size = HV_SEGMENTS*HV_SEGMENT_SIZE;

    for (size_t s = 0; s < HV_SEGMENTS; s++) {
        for (size_t n = 0; n < N; n++) {
            if (parsed_mem[n].size() != expected_hv_size) {
                throw std::runtime_error("Number of HV dimensions in HV memory file parsed is different from expected (" + std::to_string(expected_hv_size) + "). Ensure you defined the appropriate number of HV dimensions (" + std::to_string(DIM) + ") and segment size (" + std::to_string(HV_SEGMENT_SIZE) + ").");
            }

            for (size_t d = 0; d < HV_SEGMENT_SIZE; d++) {
                reshaped_mem[s][n][d] = parsed_mem[n][s*HV_SEGMENT_SIZE+d];
            }
        }
    }
}
#endif // __SYNTHESIS__
