source tcl/common.tcl

# General project variables
set RUN_SIM false
set RUN_HLS_SYNTH false
set RUN_HLS_SYNTH true
set RUN_VIVADO_SYNTH false
set RUN_VIVADO_IMPL false
set PROJECT_NAME ""
set CLK_PERIOD 10

# Define HV constants
set DIM 1000
set SEGMENT_SIZE 1000
set DIM 2048
set SEGMENT_SIZE 256
set DATAPATHS 4; # Number of parallel segment datapaths
set VSA "bsc"
set CGR_POINTS 4

# Is there a custom parameter file?
if {$argc == 1} {
    source [lindex $argv 0]
}
set valid_vsa {"bsc" "cgr"}
com_assert_in $VSA $valid_vsa

set script_path [ file dirname [ file normalize [ info script ] ] ]; # Path of this script file

set model_name "${VSA}"
set define_VSA "-D__VSA_[string toupper $VSA]__"; # Define VSA to be used in app
# Append the number of points used in CGR to the model name
if { ${VSA} == "cgr" } {
    set model_name "${model_name}${CGR_POINTS}"
}

set cflags "-D__HV_DIMENSIONS__=${DIM} -D__HV_SEGMENT_SIZE__=${SEGMENT_SIZE} -D__SEGMENT_DATAPATHS__=${DATAPATHS} ${define_VSA} -D__CGR_POINTS__=${CGR_POINTS}"

#open_project "vitis_voicehd-${model_name}-d${DIM}-seg_size${SEGMENT_SIZE}-dp${DATAPATHS}";# -reset
set proj_name "vitis_voicehd-${model_name}-d${DIM}-seg_size${SEGMENT_SIZE}-dp${DATAPATHS}"
if { $PROJECT_NAME != "" } {
    set proj_name ${PROJECT_NAME}
}
open_project ${proj_name} -reset

add_files -cflags ${cflags} -tb "src/voicehd_tb.cpp"

add_files -cflags ${cflags} "src/common.hpp"
add_files -cflags ${cflags} "src/defines.hpp"
add_files -cflags ${cflags} "src/bsc.cpp"
add_files -cflags ${cflags} "src/bsc.hpp"
add_files -cflags ${cflags} "src/cgr.cpp"
add_files -cflags ${cflags} "src/cgr.hpp"
add_files -cflags ${cflags} "src/voicehd.hpp"
add_files -cflags ${cflags} "src/voicehd.cpp"
add_files -cflags ${cflags} -tb "src/dataset.hpp"
add_files -cflags ${cflags} -tb "src/dataset.cpp"

set TOP "voicehd_enc_seg"
set_top ${TOP}
open_solution "solution1"
com_set_part
create_clock -period ${CLK_PERIOD}

# voicehd_enc_seg_dp() #
set_directive_unroll -factor ${DATAPATHS} voicehd_enc_seg/VoiceHD_Segment
set_directive_array_partition voicehd_enc_seg s_acc_dists
# Explicitely instantiate datapath
set_directive_function_instantiate voicehd_enc_seg_dp datapath_id

# Accelerator optimizations. Pick one!
#source tcl/voicehd_bsc_opt.tcl; # Vertical unrolled design
#source tcl/voicehd_bsc_bnb.tcl; # Multi-cycle encoding with fused bind-and-bundle
#source tcl/voicehd_cgr_bnb.tcl; # Multi-cycle encoding with fused bind-and-bundle for CGR
# TODO: Unify bnb design into a single file
source tcl/voicehd_${VSA}_bnb.tcl; # Multi-cycle encoding with fused bind-and-bundle for CGR

set serial_dir "${script_path}/serial"
set im_path "${serial_dir}/voicehd-${model_name}-d${DIM}-id.txt"
set cim_path "${serial_dir}/voicehd-${model_name}-d${DIM}-cim.txt"
set am_path "${serial_dir}/voicehd-${model_name}-d${DIM}-am.txt"
set ds_path "${serial_dir}/test_data.txt"
set label_path "${serial_dir}/test_label.txt"
if {bool($RUN_SIM)} {
    csim_design -argv "${im_path} ${cim_path} ${am_path} ${ds_path} ${label_path}"
}

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

