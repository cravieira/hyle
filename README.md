# Hyle

Hyle is an HLS-based framework for building HDC accelerators based on Vitis HLS. It eases the implementation of HDC hardware by providing a clean API of HDC operations for Binary Spatter Codes (BSC)[^1] and Cyclic Group Representations (CGR)[^2].
1. **Versatile**: Hyle is the first tool to accelerate the CGR class while also supporting the BSC class. Since both classes must implement the same HDC operations, they have very similar APIs. Users can quickly evaluate their design under different HDC classes.
2.  **Tested**: Each HDC operation and class provided is tested, avoiding the burden of reimplementing them.
3. **Performant**: By building accelerators upon HDC operations, a user can tune the microarchitecture generation with HLS pragmas according to their needs.
4. **Extendable**: Working at the HLS level instead of HDL (Verilog or VHDL) allows users to quickly extend the library according to their projects.

## Requirements

Hyle is built upon Xilinx's Vitis HLS, which must be installed and configured in your PATH. This README assumes the Vitis executable is named `vitis_hls`. This is important as Xilinx recently introduced the `vitis` executable with a slightly different CLI than `vitis_hls`. However, both executables should be able to reproduce the experiments in this repository.

For instructions regarding Vitis installation, please refer to the [website](https://www.amd.com/de/products/software/adaptive-socs-and-fpgas/vitis/vitis-hls.html).

## Using Hyle

Hyle provides an API of HDC operations that can be composed to build accelerators for different purposes, e.g., single application or generic. Following Vitis HLS conventions, users must write a testbench file, a design source file, and a design header file to bridge testbench and design sources. We implement example single-application accelerators for VoiceHD[^3] and HDCHOG[^4].

## Paper experiments

The paper experiments are split into accuracy evaluation and hardware evaluation.

**Accuracy**: The accuracy experiments were carried out in TorchHD, a 3rd-party library, to train models and obtain their accuracy considering different parameters. We then load the models' weights on the Hyle-based hardware testbenches to ensure correctness. The referred application experiments were used to obtain the results in Figures 2, 8, and Table IV. We keep accuracy experiments apart from this repository.

**Hardware** Hardware-related experiments are reported in Tables II, III, and Figure 9. The result in Table II presents the average cost per dimension of each HDC operation. This experiment can be reproduced by running `benchmark.sh`, which prints to the `stdout` a list of Vitis HLS commands. Each command runs the Xilinx toolchain until the Vivado Implementation step to obtain the most accurate hardware reports. Users can run each command individually or use GNU parallel[^5] (recommended) for automatic batch execution on multiple CPUs. Hyle integrates with GNU parallel (see `parallel_launch()` in [bash/common.sh](bash/common.sh)). The following snippet executes all benchmark experiments:

```bash
source bash/common.sh # Load common bash functions for experiment launching automation
./benchmark.sh > jobs_benchmark.txt # Write the list of Vitis jobs to be run into a file
parallel_launch <jobs> "$(cat jobs_benchmark.txt)" # Substitute the <jobs> parameter for the desired number of simultaneous processes. Notice that Vitis requires intensive use of RAM, which can result in abrupt process termination if <jobs> is too high.
```

Running the jobs creates several `vitis_bench-*` folders with the Vitis projects. It is possible to parse the results into a `.csv` file by running:
```bash
./parse-results.py benchmark.csv $(ls -d vitis_bench-* | sort -V)
```

The results in Table III and Figure 9 are obtained by a Design Space Exploration (DSE) with the HDCHOG accelerator.

```bash
source bash/common.sh
./dse.sh > jobs_hdchog.txt
parallel_launch <jobs> "$(cat jobs_hdchog.txt)"
```

This creates projects with `vitis_dse_hdchog-*` and `vitis_dse_voicehd-*` patterns, which can be parsed by:
```bash
./parse-results.py hdchog_dse.csv $(ls -d vitis_dse_hdchog-*  | sort -V)
./parse-results.py hdchog_dse.csv $(ls -d vitis_dse_voicehd-* | sort -V)
```

Additional scripts to plot and compile the data into tables are kept apart from this repository.

## Running unit tests

BSC and CGR classes are tested to ensure correctness. The tests and their respective runners are located according in:
```
├── src
│   ├── bsc_tb.cpp # BSC test file
│   ├── cgr_tb.cpp # CGR test file
│   ...
├── test_bsc.tcl # BSC test runner
└── test_cgr.tcl # CGR test runner
```

To run tests, simply call Vitis passing a test runner file, e.g., `vitis_hls test_bsc.tcl`.

## References
[^1]: Hyperdimensional Computing: An Introduction to Computing in Distributed Representation with High-Dimensional Random Vectors
[^2]: Understanding hyperdimensional computing for parallel single-pass learning
[^3]: VoiceHD: Hyperdimensional Computing for Efficient Speech Recognition
[^4]: HDCOG: A Lightweight Hyperdimensional Computing Framework with Feature Extraction
[^5]: https://www.gnu.org/software/parallel/

