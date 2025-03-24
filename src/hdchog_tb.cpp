#include "hdchog.hpp"

#include <cassert>
#include <cstdint>
#include <iostream>
#include <stdexcept>

#include "dataset.hpp"

void prepare_input(hog_t &hog, const data_t &entry) {
    // Convert input from [CELLS*ORIENTATIONS] to [CELLS][ORIENTATIONS]
    for (int i = 0; i < HOG_CELLS; i++) {
        for (int j = 0; j < HOG_ORIENTATIONS; j++) {
            hog[i][j] = entry[i*HOG_ORIENTATIONS+j];
        }
    }
}

int main(int argc, char *argv[]) {
    auto cell_data = parse_mem_file(argv[1]);
    cell_mem_t cell_mem;
    segment_mem(cell_data, cell_mem);

    auto ori_data = parse_mem_file(argv[2]);
    ori_mem_t ori_mem;
    segment_mem(ori_data, ori_mem);

    auto mag_data = parse_mem_file(argv[3]);
    mag_mem_t mag_mem;
    segment_mem(mag_data, mag_mem);

    auto am_data = parse_mem_file(argv[4]);
    am_t am;
    segment_mem(am_data, am);

    auto test_ds = read_dataset(argv[5]);
    auto test_labels = read_labels(argv[6]);

    if (test_ds.size() != test_labels.size()) {
        throw std::runtime_error("The size of testing dataset and labels are different. Ensure you are passing the correct dataset path.");
    }

    size_t correct = 0;
    for (size_t i = 0; i < test_ds.size(); i++) {
        hog_t input;
        prepare_input(input, test_ds[i]);
        class_t pred_label;
        hdchog(
                pred_label,
                input,
                cell_mem,
                ori_mem,
                mag_mem,
                am
            );
        if (pred_label == test_labels[i]) {
            correct++;
        }
    }

    float acc = (float)correct/(float)test_ds.size()*100.;
    std::cout << "Correct: " << correct << " Acc: " << acc << "%" << std::endl;
    return 0;
}
