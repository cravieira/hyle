# Hyle project importer for VitisHLS #
# Source this script to import hyle sources into your project.
# The following variables must be defined before sourcing hyle.tcl:
# HYLE_DIMENSIONS: (int) The number of vector dimensions in the HDC/VSA
#   application.
# HYLE_SEGMENT_SIZE: (int) The datapath width to compute each hypervector
#   segment.
#
# Additionaly, HYLE returns the following variable to the users:
# HYLE_CFLAGS: A list of compilation flags that need to be used by all source
#   files dependent on hyle.

# Find hyle's path
set HYLE_PATH [ file dirname [ file normalize [ info script ] ] ]

source "${HYLE_PATH}/tcl/common.tcl"

# Define testbench constants
set HYLE_SRC "${HYLE_PATH}/src"
set HYLE_PUBLIC_HEADERS "${HYLE_PATH}/include"

# Check if user has defined hyle's configuration parameters
if {![info exists HYLE_DIMENSIONS]} {
    puts "Please define HYLE_DIMENSIONS variable before sourcing hyle.tcl"
    exit 1
}
if {![info exists HYLE_SEGMENT_SIZE]} {
    puts "Please define HYLE_SEGMENT_SIZE variable before sourcing hyle.tcl"
    exit 1
}
if {![info exists HYLE_VSA]} {
    puts "Please define HYLE_VSA variable before sourcing hyle.tcl"
    exit 1
}
if {![info exists HYLE_CGR_POINTS]} {
    set HYLE_CGR_POINTS 4
}
set valid_vsa {"bsc" "cgr" "BSC" "CGR"}
com_assert_in $HYLE_VSA $valid_vsa

set define_VSA "-D__VSA_[string toupper $HYLE_VSA]__"; # Define VSA to be used in app

set HYLE_DEFINES "-D__HV_DIMENSIONS__=${HYLE_DIMENSIONS} -D__HV_SEGMENT_SIZE__=${HYLE_SEGMENT_SIZE} ${define_VSA} -D__CGR_POINTS__=${HYLE_CGR_POINTS}"
set HYLE_CFLAGS "-I${HYLE_PUBLIC_HEADERS} ${HYLE_DEFINES}"

# Add hyle srcs into parent project's build list
puts "${HYLE_CFLAGS}"
add_files -cflags ${HYLE_CFLAGS} "${HYLE_SRC}/bsc.cpp"
add_files -cflags ${HYLE_CFLAGS} "${HYLE_SRC}/cgr.cpp"
add_files -tb -cflags ${HYLE_CFLAGS} "${HYLE_SRC}/dataset.cpp"

