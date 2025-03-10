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
set_directive_unroll voicehd_enc_seg/Argmin

# Unroll bind
#set_directive_unroll voicehd_enc_seg_dp/SegmentBind; # Unroll parallel dimensions accumulation (vertical)

# Partition vector
set_directive_array_partition voicehd_enc_seg_dp item
set_directive_array_partition voicehd_enc_seg_dp c_item
set_directive_array_partition voicehd_enc_seg_dp bundle_acc; # TODO: This pragma seems to be ignored by the HLS tool. Remove it if that's the case.

# BSC #
# Parallel distance computation
#set_directive_inline bsc_distN
#set_directive_unroll bsc_distN/ComputeDist; # Unroll parallel dimensions accumulation (vertical)

# THE DIRECTIVE BELOW GREATLY REDUCES LATENCY!!!
set_directive_inline bsc_dist
set_directive_unroll bsc_dist/AddReduce

# bsc_bnb optimizations
# Solves the II Violation when Segment Size is 256. The problem is that
# optimizations done in the caller funcion's parameters are not being passed to
# the parameters received in bsc_bnb. Inlining solves the problem.
set_directive_inline bsc_bnb
#set_directive_inline bsc_bnb_threshold
set_directive_unroll bsc_bnb_threshold/BscBnbThreshold

# TESTES
# Datapath instantiation. Choose if datapaths are instantiated explicitely or implicitely using inlining. Mutually exclusive.
set_directive_inline voicehd_enc_seg_dp; # Commented, but can be used in the final version to increase optimizations a little
#set_directive_function_instantiate voicehd_enc_seg_dp datapath_id

set_directive_array_partition -dim 1 voicehd_enc_seg_dp features
