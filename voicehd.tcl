source tcl/common.tcl

# Define HV constants
set DIM 1000
#set DIM 32
#set DIM 16384
#set DIM 32768
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

open_project "vitis_voicehd-${model_name}-d${DIM}-seg_size${SEGMENT_SIZE}-dp${DATAPATHS}" -reset

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

set_top voicehd_enc_seg
open_solution "solution1"
com_set_part
create_clock -period 10

# voicehd_enc_seg_dp() #
set_directive_unroll -factor ${DATAPATHS} voicehd_enc_seg/VoiceHD_Segment
set_directive_array_partition voicehd_enc_seg s_acc_dists

# Accelerator optimizations. Pick one!
#source tcl/voicehd_bsc_opt.tcl; # Vertical unrolled design
#source tcl/voicehd_bsc_bnb.tcl; # Multi-cycle encoding with fused bind-and-bundle
#source tcl/voicehd_cgr_bnb.tcl; # Multi-cycle encoding with fused bind-and-bundle for CGR
source tcl/voicehd_${VSA}_bnb.tcl; # Multi-cycle encoding with fused bind-and-bundle for CGR

set serial_dir "${script_path}/serial"
set im_path "${serial_dir}/voicehd-${model_name}-d${DIM}-id.txt"
set cim_path "${serial_dir}/voicehd-${model_name}-d${DIM}-cim.txt"
set am_path "${serial_dir}/voicehd-${model_name}-d${DIM}-am.txt"
set ds_path "${serial_dir}/test_data.txt"
set label_path "${serial_dir}/test_label.txt"
csim_design -argv "${im_path} ${cim_path} ${am_path} ${ds_path} ${label_path}"
#csim_design -O -argv "${im_path} ${cim_path} ${am_path} ${ds_path} ${label_path}"
#csim_design -setup

#csynth_design -dump_cfg -synthesis_check
#csynth_design

#cosim_design -O

#export_design -flow syn
#export_design -flow impl
