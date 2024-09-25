open_project vitis_main -reset
add_file -tb "src/hdc_tb.cpp"

add_file "src/bsc.cpp"
add_file "src/bsc.hpp"

set_top bsc_bundle
open_solution "solution1"
set_part  {xc7z020clg400-1}
create_clock -period 10
csim_design
csynth_design

