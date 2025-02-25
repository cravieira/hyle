source tcl/common.tcl

set DIM 32
set CGR_POINTS 4
set VSA "cgr"; # VSA = bsc|cgr
set OPERATION "bind"; # OPERATION = bind|bnb_32|dist

# Is there a custom parameter file?
if {$argc == 1} {
    source [lindex $argv 0]
}
set valid_vsa {"bsc" "cgr"}
set valid_operation {"bind" "bnb32" "dist"}
set datapaths 1; # Number of parallel segment datapaths

com_assert_in $VSA $valid_vsa
com_assert_in $OPERATION $valid_operation

set SEGMENT_SIZE ${DIM}
set SEGMENTS [expr $DIM / $SEGMENT_SIZE]

set cflags "-D__HV_DIMENSIONS__=${DIM} -D__HV_SEGMENT_SIZE__=${SEGMENT_SIZE} -D__SEGMENT_DATAPATHS__=${datapaths} -D__CGR_POINTS__=${CGR_POINTS} -Isrc"

if { ${VSA} == "bsc"} {
    set model_name "${VSA}"
} elseif { ${VSA} == "cgr" } {
    set model_name "${VSA}${CGR_POINTS}"
}

open_project "vitis_bench-${model_name}-${OPERATION}-d${DIM}" -reset
#open_project "vitis_bench-${model_name}-${OPERATION}-d${DIM}"

add_files -cflags ${cflags} -tb "src/common.hpp"
add_files -cflags ${cflags} -tb "src/defines.hpp"

add_files -cflags ${cflags} "src/${VSA}.cpp"
add_files -cflags ${cflags} "src/${VSA}.hpp"
add_files -cflags ${cflags} "benchmark/bench_${VSA}.cpp"

open_solution "solution1"
set_part  {xczu7ev-ffvf1517-3-e}
create_clock -period 10

set_top ${VSA}_${OPERATION}

if {$VSA == "bsc"} {
    # bundle
    set_directive_unroll bsc_bundle/MajColumn; # Unroll parallel dimensions in a segment (horizontal)

    # bnb
    set_directive_unroll bsc_bnb_threshold/BscBnbThreshold
    set_directive_inline bsc_bnb
    set_directive_inline bsc_bnb_threshold

    # dist
    set_directive_unroll bsc_dist/AddReduce
}

if {$VSA == "cgr"} {
    # bundle
    set_directive_unroll cgr_bundle/AccVecReset
    set_directive_unroll cgr_bundle/CgrBundleVec
    set_directive_unroll cgr_bundle/CgrBundleVecElem
    set_directive_unroll cgr_bundle/Threshold
    # bind
    set_directive_inline cgr_bind

    # bnb
    set_directive_inline cgr_bnb_threshold
    set_directive_inline cgr_bnb
    set_directive_inline init_bnb_acc_t
    set_directive_array_partition -dim 1 cgr_bnb32 bundle_acc

    # dist
    set_directive_unroll cgr_dist/AddReduce
}

csynth_design

#export_design -flow syn
export_design -flow impl

