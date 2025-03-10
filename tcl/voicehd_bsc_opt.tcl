# Optimizations for VoiceHD accelerator. This set of pragma can greatly reduce
# latency and scale for high dimensions with multiple pipelines. This file
# relies on VoiceHD implemented with split bind and bundle stages.

# voicehd_enc_seg() #
set_directive_interface -mode ap_memory -storage_type rom_1p voicehd_enc_seg im
set_directive_interface -mode ap_memory -storage_type rom_1p voicehd_enc_seg cim
set_directive_interface -mode ap_memory -storage_type rom_1p voicehd_enc_seg am
set_directive_array_partition -dim 1 -type cyclic -factor ${DATAPATHS} voicehd_enc_seg im
set_directive_array_partition -dim 1 -type cyclic -factor ${DATAPATHS} voicehd_enc_seg cim
set_directive_array_partition -dim 1 -type cyclic -factor ${DATAPATHS} voicehd_enc_seg am
set_directive_array_partition -dim 2 voicehd_enc_seg im
set_directive_array_partition -dim 2 voicehd_enc_seg cim
set_directive_array_partition -dim 2 voicehd_enc_seg am

set_directive_function_instantiate voicehd_enc_seg_dp datapath_id
# Unroll argmin
set_directive_unroll voicehd_enc_seg/Argmin; # Unroll parallel dimensions accumulation (vertical)

# Unroll bind
set_directive_unroll voicehd_enc_vertical_unroll/SegmentBind;

# Partition vector
set_directive_array_partition -dim 1 voicehd_enc_seg_dp features
set_directive_array_partition -dim 1 voicehd_enc_vertical_unroll bound_hvs

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

