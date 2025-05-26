# Optimizations for VoiceHD accelerator with CGR class. Encode using bind and bundle.

# voicehd_enc_seg() #
# Partition IM memory
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

# Unroll bind
#set_directive_unroll voicehd_enc_seg_dp/SegmentBind; # Unroll parallel dimensions accumulation (vertical)

# Partition vector
#set_directive_array_partition voicehd_enc_bnb item
#set_directive_array_partition voicehd_enc_bnb c_item
set_directive_array_partition -dim 1 voicehd_enc_bnb bundle_acc; # TODO: This pragma seems to be ignored by the HLS tool. Remove it if that's the case.

# CGR #
# bind
set_directive_inline cgr_bind

# bnb
set_directive_inline cgr_bnb_threshold
set_directive_inline cgr_bnb
set_directive_inline cgr_init_bnb_acc_t

# dist
set_directive_inline cgr_dist
set_directive_unroll cgr_dist/AddReduce
set_directive_inline cgr_distN

# TESTS
# Datapath instantiation. Choose if datapaths are instantiated explicitely or implicitely using inlining. Mutually exclusive.
#set_directive_inline voicehd_enc_seg_dp; # Commented, but can be used in the
#final version to increase optimizations a little.
# Update: using inline voicehd_enc_seg_dp may not be the best decision as some
# configurations do not work as expected. For instance, cgr4-d1024-ss256-dp2
# results in doubled latency, as one datapath works after another and not
# simultaneously.
#set_directive_function_instantiate voicehd_enc_seg_dp datapath_id

set_directive_array_partition -dim 1 voicehd_enc_seg_dp features

