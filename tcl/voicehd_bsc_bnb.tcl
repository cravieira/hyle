# Optimizations for VoiceHD accelerator that encode using bind and bundle

# voicehd_enc_seg() #
# Partition IM memory
# Works, but takes too long to synthesize. The results are the same as with std::array
set_directive_interface -mode ap_memory -storage_type rom_1p voicehd_enc_seg im
set_directive_interface -mode ap_memory -storage_type rom_1p voicehd_enc_seg cim
set_directive_interface -mode ap_memory -storage_type rom_1p voicehd_enc_seg am
set_directive_array_partition -dim 1 -type cyclic -factor ${DATAPATHS} voicehd_enc_seg im
set_directive_array_partition -dim 1 -type cyclic -factor ${DATAPATHS} voicehd_enc_seg cim
set_directive_array_partition -dim 1 -type cyclic -factor ${DATAPATHS} voicehd_enc_seg am
#set_directive_array_partition -dim 2 voicehd_enc_seg im; # needs to be on dim 2?
#set_directive_array_partition -dim 2 voicehd_enc_seg cim; # needs to be on dim 2?
#set_directive_array_partition -dim 2 voicehd_enc_seg am; # needs to be on dim 2?

set_directive_unroll voicehd_enc_seg/ResetAccumulators; # Parallel reset accumulator banks

# Unroll argmin
#set_directive_unroll voicehd_enc_seg/Argmin; # Turned off because cgr optimization file does not use this

# Unroll bind
#set_directive_unroll voicehd_enc_seg_dp/SegmentBind; # Unroll parallel dimensions accumulation (vertical)

# Partition vector
set_directive_array_partition voicehd_enc_bnb bundle_acc; # TODO: new pragma

# BSC #
# bind
set_directive_inline bsc_bind

# bnb
set_directive_inline bsc_bnb_threshold
set_directive_inline bsc_bnb
set_directive_inline bsc_init_bnb_acc_t

# dist
set_directive_inline bsc_dist
set_directive_unroll bsc_dist/AddReduce
set_directive_inline bsc_distN

# Parallel distance computation
#set_directive_inline bsc_distN
#set_directive_unroll bsc_distN/ComputeDist; # Unroll parallel dimensions accumulation (vertical)

# TESTES
# Datapath instantiation. Choose if datapaths are instantiated explicitely or implicitely using inlining. Mutually exclusive.
#set_directive_inline voicehd_enc_seg_dp; # Commented, but can be used in the final version to increase optimizations a little
set_directive_function_instantiate voicehd_enc_seg_dp datapath_id

set_directive_array_partition -dim 1 voicehd_enc_seg_dp features
