# Run unit-tests on BSC operations.
open_project vitis_test_bsc -reset

# Define testbench constants
set cflags "-D__HV_DIMENSIONS__=10 -D__HV_SEGMENT_SIZE__=10"

add_files -tb -cflags ${cflags} "src/bsc_tb.cpp"
add_files -tb -cflags ${cflags} "src/common.hpp"
add_files -tb -cflags ${cflags} "src/defines.hpp"

add_files -cflags ${cflags} "src/bsc.cpp"
add_files -cflags ${cflags} "src/bsc.hpp"

set_top bsc_bundle; # Set any top function
open_solution "solution1"
set_part  {xc7z020clg400-1}
create_clock -period 10
csim_design
