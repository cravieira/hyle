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
