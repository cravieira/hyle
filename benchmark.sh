#!/bin/bash

# Run several synth benchmarks to evaluate HLS accelerators

set -eu

source bash/common.sh

JOBS=4 # Number of parallel jobs to be executed

DIMS="32 64 128 256 "
VSAS="bsc cgr-4 cgr-8 cgr-16"
OPERATIONS="bind bnb32 dist"
MAIN_TCL="synth_benchmark.tcl"

BENCHMARK_TEMP_DIR="_tmp_benchmark"
VITIS_PROJECT_PREFIX="vitis_bench-"

# Vitis-related helper functions
launch_vitis() {
    echo "vitis_hls $@\n"
}

# Temporary files management
TMP_DIR=""
clean_up() {
    rm -rf $BENCHMARK_TEMP_DIR
}

mk_temp_dir() {
    rm -rf $BENCHMARK_TEMP_DIR
    mkdir -p $BENCHMARK_TEMP_DIR
}
#########

parse_params() {
    tcl_content=""
    while [[ $# -gt 0 ]]; do
        case $1 in
            "-dim")
                tcl_content+="set DIM $2\n"
                shift # past argument
                shift # past value
                ;;
            "-vsa")
                vsa_class=$2
                if [[ $2 == "cgr"* ]]; then
                    IFS="-" read -r vsa_class cgr_points <<< "$2"
                    tcl_content+="set CGR_POINTS $cgr_points\n"
                fi
                tcl_content+="set VSA $vsa_class\n"
                shift # past argument
                shift # past value
                ;;
            "-op")
                tcl_content+="set OPERATION $2\n"
                shift # past argument
                shift # past value
                ;;
            "*")
                echo "Unknown option $1"
                exit 1
                ;;
        esac
    done
    echo $tcl_content
}

launch_synth() {
    param_file=$(mktemp $BENCHMARK_TEMP_DIR/bench_params.XXXX)
    file_content=$(parse_params "$@")
    printf "${file_content}" > $param_file
    echo $(launch_vitis $MAIN_TCL $param_file)
}

benchmark_operations() {
    mk_temp_dir
    cmd=""
    for vsa in $VSAS ; do
        for dim in $DIMS ; do
            for operation in $OPERATIONS ; do
                cmd+=$(launch_synth -dim $dim -vsa $vsa -op $operation)
            done
        done
    done
    printf "$cmd"
}

cmd=$(benchmark_operations)
printf "$cmd"
#parallel_launch "$JOBS" "$cmd"

#clean_up
