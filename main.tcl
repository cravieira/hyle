open_project vitis_main -reset
add_file -tb "src/hdc_tb.cpp"

open_solution "solution1"
csim_design
