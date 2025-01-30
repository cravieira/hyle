# Optimizations for VoiceHD accelerator. This set of pragma can greatly reduce latency and scale for high dimensions with multiple pipelines.

# voicehd_enc_seg() #
# Partition IM memory
# Works, but takes too long to synthesize. The results are the same as with std::array
set_directive_interface -mode ap_memory -storage_type rom_1p voicehd_enc_seg im
set_directive_interface -mode ap_memory -storage_type rom_1p voicehd_enc_seg cim
set_directive_interface -mode ap_memory -storage_type rom_1p voicehd_enc_seg am
set_directive_array_partition -dim 1 -type cyclic -factor ${datapaths} voicehd_enc_seg im
set_directive_array_partition -dim 1 -type cyclic -factor ${datapaths} voicehd_enc_seg cim
set_directive_array_partition -dim 1 -type cyclic -factor ${datapaths} voicehd_enc_seg am
set_directive_array_partition -dim 2 voicehd_enc_seg im
set_directive_array_partition -dim 2 voicehd_enc_seg cim
set_directive_array_partition -dim 2 voicehd_enc_seg am

# Unroll argmin
set_directive_unroll voicehd_enc_seg/Argmin; # Unroll parallel dimensions accumulation (vertical)

# voicehd_enc_seg_dp() #
set_directive_array_partition voicehd_enc_seg s_acc_dists
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

