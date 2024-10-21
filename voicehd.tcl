# Define HV constants
set DIM 10
set DIM 1000
set SEGMENT_SIZE 5
set cflags "-D__HV_DIMENSIONS__=${DIM} -D__HV_SEGMENT_SIZE__=${SEGMENT_SIZE}"

open_project "vitis_voicehd-d${DIM}-seg_size${SEGMENT_SIZE}" -reset
#open_project vitis_voicehd

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
csim_design
#csim_design -O
csynth_design
#cosim_design -O

export_design -flow syn
