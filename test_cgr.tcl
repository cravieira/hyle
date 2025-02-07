# Run unit-tests on CGR operations.
open_project vitis_test_cgr -reset

# Define testbench constants
set cflags "-D__HV_DIMENSIONS__=10 -D__HV_SEGMENT_SIZE__=10"

add_files -tb -cflags ${cflags} "src/cgr_tb.cpp"
add_files -tb -cflags ${cflags} "src/common.hpp"
add_files -tb -cflags ${cflags} "src/defines.hpp"

add_files -cflags ${cflags} "src/cgr.cpp"
add_files -cflags ${cflags} "src/cgr.hpp"

set_top cgr_bind; # Set any top function
open_solution "solution1"
set_part  {xc7z020clg400-1}
create_clock -period 10
csim_design

#csynth_design
#export_design -flow syn
