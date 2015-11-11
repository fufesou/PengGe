cmake_minimum_required (VERSION 2.6)
project (sample_account)
include (../common)

# The version number
set (AM_VERSION_MAJOR 1)
set (AM_VERSION_MINOR 0)

# configure a header file to pass some of the CMake settings
# to source file
configure_file (
	"${PROJECT_SROUCE_DIR}/amconfig.h.in"
	"${PROJECT_BINARY_DIR}/amconfig.h"
	)

# add the binary tree to the search path for include file
# so that we will find amconfig.h
include_directories ("${PROJECT_BINARY_DIR}")

# should we use our own math functions?
# option (USE_MYMATH
#	"Use tutorial provided math implementation" ON)

# add the MathFunctions library?
#
#if (USE_MYMATH)
#	include_directories ("${PROJECT_SOURCE_DIR}/MathFunctions")
#	add_subdirectory (MathFunctions)
#	set (EXTRA_LIBS ${EXTRA_LIBS} MathFunctions)
#endif (USE_MYMATH)

add_executable(testam testam.c)
# target_link_libraries (testam ${EXTRA_LIBS})
