#!/bin/bash

# Run several hdchog benchmarks to evaluate HLS accelerators for different HDC
# classes

set -eu

source bash/common.sh

JOBS=4 # Number of parallel jobs to be executed

VSAS=""
DIMS=""
DATAPATHS=""
SEGMENT_SIZE=""
CLOCK_PERIODS="5 10 15"

MAIN_TCL="hdchog.tcl"
PARAM_TEMP_DIR="_tmp_hdchog"

proj_name() {
    local vsa=$1
    local dim=$2
    local ss=$3
    local dp=$4
    local cp=$5

    local model_name=$vsa
    if [[ $model_name == "cgr"* ]]; then
        IFS="-" read -r vsa_class cgr_points <<< "$vsa"
        model_name="$vsa_class$cgr_points"
    fi

    echo "vitis_dse_hdchog-${model_name}-d${dim}-seg_size${ss}-dp${dp}-cp${cp}"
}

dse() {
    mkdir -p $PARAM_TEMP_DIR
    cmd=""
    for vsa in $VSAS ; do
        for dim in $DIMS ; do
            for ss in $SEGMENT_SIZE; do
                for dp in $DATAPATHS ; do
                    for cp in $CLOCK_PERIODS ; do
                        p_name=$(proj_name $vsa $dim $ss $dp $cp)
                        cmd+=$(com_launch_synth "$MAIN_TCL" "$PARAM_TEMP_DIR" -dim $dim -seg-size "$ss" -vsa $vsa -datapath $dp -clk-period $cp -hls-synth "true" -vivado-impl "true" -project-name $p_name)
                    done
                done
            done
        done
    done
    echo "$cmd"
}

cmd=""
# CGR
VSAS="cgr-4"
DIMS="2048"
SEGMENT_SIZE=256
DATAPATHS="1 2 4 8"
cmd+=$(dse)

# Narrower CGR datapaths
DATAPATHS="1"
SEGMENT_SIZE=128
cmd+=$(dse)
SEGMENT_SIZE=64 # This design is too small compared to the BSC designs evaluated
cmd+=$(dse)

# BSC
VSAS="bsc"
DIMS="8192 16384 32768"
DIMS="16384 32768"
SEGMENT_SIZE=256
DATAPATHS="1 2 4 8 16"
#DATAPATHS="32" # Failing in Vivado implementation due to high number of IOs in the IP
cmd+=$(dse)

printf "$cmd"
