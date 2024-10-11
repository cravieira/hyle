#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

#include "bsc.hpp"
#include "voicehd.hpp"

// Dataset loaders
typedef std::vector<feat_t> data_t;
typedef std::vector<data_t> dataset_t;
typedef std::vector<class_t> label_t;

dataset_t read_dataset(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) {
        throw std::runtime_error("Unable to open dataset file: " + path);
    }

    dataset_t dataset;
    std::string line;
    while (std::getline(f, line)) {
        float i;
        data_t data;
        std::stringstream ss(line);
        while (ss >> i) {
            data.emplace_back(i);
            if (ss.peek() == ',') {
                ss.ignore();
            }
        }
        dataset.emplace_back(data);
    }

    return dataset;
}

label_t read_labels(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open()) {
        std::cerr << "Could not open file: " << path << std::endl;
        assert(f.is_open());
    }

    label_t labels;
    std::string line;
    while (std::getline(f, line)) {
        labels.emplace_back(std::stoi(line));
    }

    return labels;
}

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
            hv[i] = static_cast<bin_t>(buffer[i]);
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
    std::array<hv_t, VOICEHD_FEATURES> im = init_mem<VOICEHD_FEATURES>("/home/zelda/Documents/git/vitis/hdc-hls/serial/bsc-d10-id.txt");
    std::array<hv_t, VOICEHD_LEVELS>  cim = init_mem<VOICEHD_LEVELS>("/home/zelda/Documents/git/vitis/hdc-hls/serial/bsc-d10-cim.txt");;
    std::array<hv_t, VOICEHD_CLASSES> am = init_mem<VOICEHD_CLASSES>("/home/zelda/Documents/git/vitis/hdc-hls/serial/bsc-d10-am.txt");
    //std::array<hv_t, VOICEHD_FEATURES> im = init_mem<VOICEHD_FEATURES>("/home/zelda/Documents/git/vitis/hdc-hls/serial/bsc-d1000-id.txt");
    //std::array<hv_t, VOICEHD_LEVELS>  cim = init_mem<VOICEHD_LEVELS>("/home/zelda/Documents/git/vitis/hdc-hls/serial/bsc-d1000-cim.txt");;
    //std::array<hv_t, VOICEHD_CLASSES> am = init_mem<VOICEHD_CLASSES>("/home/zelda/Documents/git/vitis/hdc-hls/serial/bsc-d1000-am.txt");
    //std::array<hv_t, VOICEHD_FEATURES> im = init_mem<VOICEHD_FEATURES>("/home/zelda/Documents/git/vitis/hdc-hls/serial/bsc-d32768-id.txt");
    //std::array<hv_t, VOICEHD_LEVELS>  cim = init_mem<VOICEHD_LEVELS>("/home/zelda/Documents/git/vitis/hdc-hls/serial/bsc-d32768-cim.txt");;
    //std::array<hv_t, VOICEHD_CLASSES> am = init_mem<VOICEHD_CLASSES>("/home/zelda/Documents/git/vitis/hdc-hls/serial/bsc-d32768-am.txt");

    auto test_ds = read_dataset("/home/zelda/Documents/git/vitis/hdc-hls/serial/test_data.txt");
    auto test_labels = read_labels("/home/zelda/Documents/git/vitis/hdc-hls/serial/test_label.txt");

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
        voicehd_enc(pred_label, features, im, cim, am);
        if (pred_label == test_labels[i]) {
            correct++;
        }
    }

    float acc = (float)correct/(float)test_ds.size()*100.;
    std::cout << "Correct: " << correct << " Acc: " << acc << "%" << std::endl;

    return 0;
}
