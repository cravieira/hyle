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

benchmark_operations() {
    mkdir -p $BENCHMARK_TEMP_DIR
    cmd=""
    for vsa in $VSAS ; do
        for dim in $DIMS ; do
            for operation in $OPERATIONS ; do
                cmd+=$(com_launch_synth "$MAIN_TCL" "$BENCHMARK_TEMP_DIR" -dim $dim -vsa $vsa -op $operation)
            done
        done
    done
    echo "$cmd"
}

cmd=$(benchmark_operations)
printf "$cmd"
#parallel_launch "$JOBS" "$cmd"

