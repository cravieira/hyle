# Define HV constants
set DIM 10
set DIM 1024
set SEGMENT_SIZE 1
set datapaths 1; # Number of parallel segment datapaths

if { $argc != 0 } {
    set datapaths [lindex $argv 0]
}

puts "Using ${datapaths} datapaths"
puts "argv ${argv} argc ${argc}"

set SEGMENTS [expr $DIM / $SEGMENT_SIZE]
set cflags "-D__HV_DIMENSIONS__=${DIM} -D__HV_SEGMENT_SIZE__=${SEGMENT_SIZE}"

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

# voicehd_enc_seg() #
# Partition IM memory
# Works, but takes too long to synthesize. The results are the same as with std::array
set_directive_array_partition -dim 2 voicehd_enc_seg im
set_directive_array_partition -dim 2 voicehd_enc_seg cim
set_directive_array_partition -dim 2 voicehd_enc_seg am

# Unroll argmin
set_directive_unroll voicehd_enc_seg/Argmin; # Unroll parallel dimensions accumulation (vertical)

# Unroll bind
set_directive_unroll voicehd_enc_seg_dp/SegmentBind; # Unroll parallel dimensions accumulation (vertical)

# Partition vector
set_directive_array_partition -dim 1 voicehd_enc_seg_dp features
set_directive_array_partition -dim 1 voicehd_enc_seg_dp bound_hvs

# BSC #
# Parallel distance computation
#set_directive_inline bsc_distN
set_directive_unroll bsc_distN/ComputeDist; # Unroll parallel dimensions accumulation (vertical)

# THE DIRECTIVE BELOW GREATLY REDUCES LATENCY!!!
set_directive_inline bsc_dist

# bsc_bundleN
set_directive_inline bsc_bundleN
set_directive_unroll bsc_bundleN/BundleN_MajColumn; # Unroll parallel dimensions in a segment (horizontal)
set_directive_unroll bsc_bundleN/BundleN_AccColumn; # Unroll parallel dimensions accumulation (vertical)

# bsc_bind
set_directive_inline bsc_bind

#csim_design -O

csynth_design
#cosim_design -O

#export_design -flow syn
