open_project vitis_main -reset

# Define testbench constants
set cflags "-D__HV_DIMENSIONS__=10 -D__HV_SEGMENT_SIZE__=10"

add_files -tb -cflags ${cflags} "src/hdc_tb.cpp"
add_files -tb -cflags ${cflags} "src/common.hpp"
add_files -tb -cflags ${cflags} "src/defines.hpp"

add_files -cflags ${cflags} "src/bsc.cpp"
add_files -cflags ${cflags} "src/bsc.hpp"

set_top bsc_bundle
open_solution "solution1"
set_part  {xc7z020clg400-1}
create_clock -period 10
csim_design
