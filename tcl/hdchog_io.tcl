# Customize HDCHOG interface
set_directive_interface -mode ap_memory -storage_type rom_1p ${TOP} cell_mem
set_directive_interface -mode ap_memory -storage_type rom_1p ${TOP} ori_mem
set_directive_interface -mode ap_memory -storage_type rom_1p ${TOP} mag_mem
set_directive_interface -mode ap_memory -storage_type rom_1p ${TOP} am
set_directive_array_partition -dim 1 -type cyclic -factor ${DATAPATHS} ${TOP} cell_mem
set_directive_array_partition -dim 1 -type cyclic -factor ${DATAPATHS} ${TOP} ori_mem
set_directive_array_partition -dim 1 -type cyclic -factor ${DATAPATHS} ${TOP} mag_mem
set_directive_array_partition -dim 1 -type cyclic -factor ${DATAPATHS} ${TOP} am
set_directive_array_partition -dim 0 ${TOP} features

