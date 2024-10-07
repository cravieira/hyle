open_project vitis_voicehd -reset
add_file -tb "src/voicehd_tb.cpp"
add_file -tb "src/common.hpp"

add_file "src/bsc.cpp"
add_file "src/bsc.hpp"
add_file "src/voicehd.cpp"
add_file "src/voicehd.hpp"

set_top voicehd
open_solution "solution1"
set_part  {xc7z020clg400-1}
create_clock -period 10
csim_design
csynth_design

export_design -flow syn
