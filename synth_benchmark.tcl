set DIM 32
set SEGMENT_SIZE ${DIM}
set datapaths 1; # Number of parallel segment datapaths
set vsa "bsc"; # vsa = bsc|cgr
set valid_vsa {"bsc" "cgr"}
set operation "bnb32"; # operation = bind|bnb_32|dist
set valid_operation {"bind" "bnb32" "dist"}

if { $argc != 0 } {
    set datapaths [lindex $argv 0]
}

puts "Using ${datapaths} datapaths"
puts "argv ${argv} argc ${argc}"

set SEGMENTS [expr $DIM / $SEGMENT_SIZE]
set CGR_POINTS 4
set cflags "-D__HV_DIMENSIONS__=${DIM} -D__HV_SEGMENT_SIZE__=${SEGMENT_SIZE} -D__SEGMENT_DATAPATHS__=${datapaths} -D__CGR_POINTS__=${CGR_POINTS} -Isrc"

if { ${vsa} == "bsc"} {
    set model_name "${vsa}"
} elseif { ${vsa} == "cgr" } {
    set model_name "${vsa}${CGR_POINTS}"
} else {
    puts "Invalid VSA class: \"${vsa}\""
    exit 1
}

if {[lsearch -exact $valid_operation $operation] == -1} {
    puts "Invalid operation: \"${operation}\""
    exit 1
}

open_project "vitis_bench-${model_name}-${operation}-d${DIM}-seg_size${SEGMENT_SIZE}-dp${datapaths}" -reset

add_files -cflags ${cflags} -tb "src/common.hpp"
add_files -cflags ${cflags} -tb "src/defines.hpp"

add_files -cflags ${cflags} "src/${vsa}.cpp"
add_files -cflags ${cflags} "src/${vsa}.hpp"
add_files -cflags ${cflags} "benchmark/bench_${vsa}.cpp"


open_solution "solution1"
set_part  {xc7z020clg400-1}
create_clock -period 10

set_top ${vsa}_${operation}

if {$vsa == "bsc"} {
    # bundle
    set_directive_unroll bsc_bundle/MajColumn; # Unroll parallel dimensions in a segment (horizontal)

    # bnb
    set_directive_unroll bsc_bnb_threshold/BscBnbThreshold
    set_directive_inline bsc_bnb
    set_directive_inline bsc_bnb_threshold

    # dist
    set_directive_unroll bsc_dist/AddReduce
}

if {$vsa == "cgr"} {
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
    set_directive_array_partition -dim 1 cgr_bnb_32 bundle_acc

    # dist
    set_directive_unroll cgr_dist/AddReduce
}

csynth_design

export_design -flow syn

