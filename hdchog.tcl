source tcl/common.tcl

# General project variables
set RUN_SIM false
set RUN_HLS_SYNTH false
set RUN_VIVADO_SYNTH false
set RUN_VIVADO_IMPL false
set PROJECT_NAME ""
set CLK_PERIOD 10

# Define HV constants
set DIM 1000
set SEGMENT_SIZE 1000
set DATAPATHS 1; # Number of parallel segment datapaths
set VSA "bsc"
set CGR_POINTS 4
set script_path [ file dirname [ file normalize [ info script ] ] ]; # Path of this script file

if { $argc != 0 } {
    set DATAPATHS [lindex $argv 0]
}

puts "Using ${DATAPATHS} datapaths"
puts "argv ${argv} argc ${argc}"

set SEGMENTS [expr $DIM / $SEGMENT_SIZE]

set valid_VSA {"bsc" "cgr"}
com_assert_in $VSA $valid_VSA

set model_name "${VSA}"
set define_VSA "-D__VSA_[string toupper $VSA]__"; # Define VSA to be used in app
# Append the number of points used in CGR to the model name
if { ${VSA} == "cgr" } {
    set model_name "${model_name}${CGR_POINTS}"
}

set cflags "-D__HV_DIMENSIONS__=${DIM} -D__HV_SEGMENT_SIZE__=${SEGMENT_SIZE} -D__SEGMENT_DATAPATHS__=${DATAPATHS} ${define_VSA} -D__CGR_POINTS__=${CGR_POINTS}"

open_project "vitis_hdchog-${model_name}-d${DIM}-seg_size${SEGMENT_SIZE}-dp${DATAPATHS}" -reset
set proj_name "vitis_hdchog-${model_name}-d${DIM}-seg_size${SEGMENT_SIZE}-dp${DATAPATHS}"
if { $PROJECT_NAME != "" } {
    set proj_name ${PROJECT_NAME}
}
open_project ${proj_name} -reset

add_files -cflags ${cflags} -tb "src/hdchog_tb.cpp"

add_files -cflags ${cflags} "src/common.hpp"
add_files -cflags ${cflags} "src/defines.hpp"
add_files -cflags ${cflags} "src/bsc.cpp"
add_files -cflags ${cflags} "src/bsc.hpp"
add_files -cflags ${cflags} "src/cgr.cpp"
add_files -cflags ${cflags} "src/cgr.hpp"
add_files -cflags ${cflags} "src/hdchog.hpp"
add_files -cflags ${cflags} "src/hdchog.cpp"
add_files -cflags ${cflags} -tb "src/dataset.hpp"
add_files -cflags ${cflags} -tb "src/dataset.cpp"

set TOP "hdchog"
set_top ${TOP}
open_solution "solution1"
com_set_part
create_clock -period ${CLK_PERIOD}

# voicehd_enc_seg_dp() #
set_directive_unroll -factor ${DATAPATHS} hdchog/HDCHOG_Segment
set_directive_array_partition hdchog s_acc_dists
# Explicitely instantiate datapath
set_directive_function_instantiate hdchog_dp datapath_id

# Accelerator optimizations. Pick one!
source tcl/hdchog_no_pipeline.tcl; # No pipeline

set serial_dir "${script_path}/serial"
set cell_hv_path "${serial_dir}/hdchog-${model_name}-d${DIM}-cell_hv.txt"
set ori_hv_path "${serial_dir}/hdchog-${model_name}-d${DIM}-ori_hv.txt"
set mag_hv_path "${serial_dir}/hdchog-${model_name}-d${DIM}-mag_hv.txt"
set am_path "${serial_dir}/hdchog-${model_name}-d${DIM}-am.txt"
set ds_path "${serial_dir}/hdchog-test_data.txt"
set label_path "${serial_dir}/hdchog-test_label.txt"
if {bool($RUN_SIM)} {
    csim_design -argv "${cell_hv_path} ${ori_hv_path} ${mag_hv_path} ${am_path} ${ds_path} ${label_path}"
}
#csim_design -setup

#csynth_design -dump_cfg -synthesis_check
if {bool($RUN_HLS_SYNTH)} {
    csynth_design
}

#cosim_design -O

if {bool($RUN_VIVADO_SYNTH)} {
    export_design -flow syn
}
if {bool($RUN_VIVADO_IMPL)} {
    export_design -flow impl
}

