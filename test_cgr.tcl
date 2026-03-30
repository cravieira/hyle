# Run unit-tests on CGR operations.
open_project vitis_test_cgr -reset

# Define testbench constants
set HYLE_VSA "cgr"
set HYLE_DIMENSIONS 10
set HYLE_SEGMENT_SIZE 10

source ./hyle.tcl

add_files -tb -cflags ${HYLE_CFLAGS} "src/cgr_tb.cpp"

set_top cgr_bind; # Set any top function
open_solution "solution1"
set_part  {xc7z020clg400-1}
create_clock -period 10
csim_design
