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

APP_NAME="" # Name of app to be executed
MAIN_TCL="" # Name of tcl script
PARAM_TEMP_DIR="_tmp_dse" # Temporary dir for parameter files

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

    echo "vitis_dse_${APP_NAME}-${model_name}-d${dim}-seg_size${ss}-dp${dp}-cp${cp}"
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

cgr_dse() {
    local dse_cmds=""
    # CGR
    VSAS="cgr-4"
    DIMS="2048"
    SEGMENT_SIZE=256
    DATAPATHS="1 2 4 8"
    dse_cmds+=$(dse)

    # Narrower CGR datapaths
    DATAPATHS="1"
    SEGMENT_SIZE=128
    dse_cmds+=$(dse)
    SEGMENT_SIZE=64 # This design is too small compared to the BSC designs evaluated
    dse_cmds+=$(dse)

    echo "$dse_cmds"
}

bsc_dse() {
    local dse_cmds=""

    # BSC
    VSAS="bsc"
    DIMS="8192 16384 32768"
    DIMS="16384 32768"
    SEGMENT_SIZE=256
    DATAPATHS="1 2 4 8 16"
    #DATAPATHS="32" # Failing in Vivado implementation due to high number of IOs in the IP
    dse_cmds+=$(dse)

    echo "$dse_cmds"
}

APP_NAME="hdchog"
MAIN_TCL="${APP_NAME}.tcl"
cmd+=""
cmd+=$(cgr_dse)
cmd+=$(bsc_dse)

APP_NAME="voicehd"
MAIN_TCL="${APP_NAME}.tcl"
cmd+=""
cmd+=$(cgr_dse)
cmd+=$(bsc_dse)

printf "$cmd"
