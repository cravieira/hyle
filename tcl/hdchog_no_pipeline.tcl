# Optimizations for HDCHOG accelerator that encode using bind and bundle and
# disable automatic pipeline generation

# hdchog #
# Customize top module IO
source tcl/hdchog_io.tcl

# Tune encoding
# top ${VSA}_enc_grad_hv #
set_directive_pipeline -off ${VSA}_enc_grad_hv/GradBnb
set_directive_pipeline -off ${VSA}_enc_mat_hv/CellBnb

# Inline sub-encode functions
set_directive_inline ${VSA}_enc_grad_hv
set_directive_inline ${VSA}_enc_cell_hv
set_directive_bind_storage -type ram_1p hdchog_enc_bsc grad_hvs
#set_directive_array_partition_ -dim 1 ram_1p hdchog_enc_bsc grad_hvs

set_directive_inline ${VSA}_bnb_threshold
set_directive_inline ${VSA}_bnb
set_directive_inline ${VSA}_init_bnb_acc_t
set_directive_inline ${VSA}_bind

# Tune dist computation after encoding
# dist
set_directive_inline ${VSA}_dist
set_directive_unroll ${VSA}_dist/AddReduce
set_directive_inline ${VSA}_distN
