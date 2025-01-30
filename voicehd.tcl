# Define HV constants
#set DIM 10
set DIM 1024
#set DIM 16384
#set DIM 32768
set SEGMENT_SIZE 1
set datapaths 1; # Number of parallel segment datapaths

if { $argc != 0 } {
    set datapaths [lindex $argv 0]
}

puts "Using ${datapaths} datapaths"
puts "argv ${argv} argc ${argc}"

set SEGMENTS [expr $DIM / $SEGMENT_SIZE]
set cflags "-D__HV_DIMENSIONS__=${DIM} -D__HV_SEGMENT_SIZE__=${SEGMENT_SIZE} -D__SEGMENT_DATAPATHS__=${datapaths}"

open_project "vitis_voicehd-d${DIM}-seg_size${SEGMENT_SIZE}-dp${datapaths}" -reset

add_files -cflags ${cflags} -tb "src/voicehd_tb.cpp"
add_files -cflags ${cflags} -tb "src/common.hpp"
add_files -cflags ${cflags} -tb "src/defines.hpp"

add_files -cflags ${cflags} "src/bsc.cpp"
add_files -cflags ${cflags} "src/bsc.hpp"
add_files -cflags ${cflags} "src/voicehd.cpp"
add_files -cflags ${cflags} "src/voicehd.hpp"

set_top voicehd_enc_seg
open_solution "solution1"
set_part  {xc7z020clg400-1}
create_clock -period 10

set_directive_unroll -factor ${datapaths} voicehd_enc_seg/VoiceHD_Segment
set_directive_function_instantiate voicehd_enc_seg_dp datapath_id

source tcl/voicehd_opt.tcl

#csim_design -O
#csim_design -setup
#csim_design

csynth_design
#cosim_design -O

#export_design -flow syn
