#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

#include "defines.hpp"
#include "voicehd.hpp"
#include "dataset.hpp"

// Dataset loaders
// Memory initializers
std::vector<hv_t> read_mem(
        const std::string &path
        ) {
    // Parse line and create HV from it
    std::ifstream f(path);
    if (!f.is_open()) {
        throw std::runtime_error("Unable to open file: " + path);
    }

    std::vector<hv_t> mem;
    std::string line;

    // Parse a single line
    while (std::getline(f, line)) {
        int32_t i; // Element buffer
        std::vector<int32_t> buffer; // Line buffer

        std::stringstream ss(line);
        while (ss >> i) {
            buffer.emplace_back(i);
        }

        // Convert the read data into an HV and push it to the memory container
        hv_t hv;
        if (buffer.size() != DIM) {
            std::string msg = "Failed to parse file " + path +". Number of read dimensions (" + std::to_string(buffer.size()) + ") is not equal to constant DIM (" + std::to_string(DIM) + ")";
            throw std::runtime_error(msg);
        }

        for (size_t i = 0; i < DIM; i++) {
            hv[i] = static_cast<hv_elem_t>(buffer[i]);
        }

        mem.emplace_back(hv);
    }

    return mem;
}

template<size_t N>
std::array<hv_t, N> init_mem(const std::string &path) {
    // Read a HDC memory file
    auto file_data = read_mem(path);
    if (file_data.size() != N) {
        std::string msg = "Number of HVs read from file \"" + path + "\" is different from the expected: " + std::to_string(N);
        throw std::runtime_error(msg);
    }

    // Convert the std::vector data to hls::vector
    std::array<hv_t, N> mem;
    for (int i = 0; i < N; i++) {
        mem[i] = file_data[i];
    }

    return mem;
}

int main(int argc, char *argv[]) {
    auto im_data = parse_mem_file(argv[1]);
    hv_t im[HV_SEGMENTS][VOICEHD_FEATURES];
    segment_mem<VOICEHD_FEATURES>(im_data, im);

    auto cim_data = parse_mem_file(argv[2]);
    hv_t cim[HV_SEGMENTS][VOICEHD_LEVELS];
    segment_mem<VOICEHD_LEVELS>(cim_data, cim);

    auto am_data = parse_mem_file(argv[3]);
    hv_t am[HV_SEGMENTS][VOICEHD_CLASSES];
    segment_mem<VOICEHD_CLASSES>(am_data, am);

    auto test_ds = read_dataset(argv[4]);
    auto test_labels = read_labels(argv[5]);

    if (test_ds.size() != test_labels.size()) {
        throw std::runtime_error("The size of testing dataset and labels are different. Ensure you are passing the correct dataset path.");
    }

    size_t correct = 0;
    for (size_t i = 0; i < test_ds.size(); i++) {
        class_t pred_label;
        feat_vec_t features;
        std::copy(
                test_ds[i].cbegin(),
                test_ds[i].cbegin()+VOICEHD_FEATURES,
                features.begin()
            );
        voicehd_enc_seg(pred_label, features, im, cim, am);
        if (pred_label == test_labels[i]) {
            correct++;
        }
    }

    float acc = (float)correct/(float)test_ds.size()*100.;
    std::cout << "Correct: " << correct << " Acc: " << acc << "%" << std::endl;
    return 0;
}
