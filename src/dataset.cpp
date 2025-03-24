#include "dataset.hpp"

#include <cassert>
#include <cstddef>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

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

mem_data_t parse_mem_file(const std::string &path) {
    std::ifstream f(path);
    if (!f.is_open()) {
        throw std::runtime_error("Unable to open HV memory file: " + path);
    }

    mem_data_t mem_data;
    std::string line;
    while (std::getline(f, line)) {
        int i;
        std::vector<hv_elem_t> buffer;
        std::stringstream ss(line);
        while (ss >> i) {
            buffer.emplace_back(static_cast<hv_elem_t>(i));
            if (ss.peek() == ',') {
                ss.ignore();
            }
        }
        mem_data.emplace_back(buffer);
    }

    return mem_data;
}
