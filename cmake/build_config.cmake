# Author: Félix C. Morency
# 2011.10

if(MSVC)
    set(VS_WARNING_LEVEL "${VS_WARNING_LEVEL}")
    set(DEBUG_FLAGS "${VS_WARNING_LEVEL} /EHsc")
    set(RELEASE_FLAGS "/EHsc")
else(MSVC)
    set(DEBUG_FLAGS "-g -O0 -Wall")
    set(RELEASE_FLAGS "-O3 -Wall")
endif(MSVC)

set(CMAKE_CXX_FLAGS_RELEASE ${RELEASE_FLAGS})
set(CMAKE_C_FLAGS_RELEASE ${RELEASE_FLAGS})

set(CMAKE_CXX_FLAGS_DEBUG "${DEBUG_FLAGS} -D_DEBUG")
set(CMAKE_C_FLAGS_DEBUG "${DEBUG_FLAGS} -D_DEBUG")

if (CMAKE_CXX_FLAGS_DEBUG MATCHES "/W[0-4]")
     string(REGEX REPLACE "/W[0-4]" "${VS_WARNING_LEVEL}" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
else ()
     set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${VS_WARNING_LEVEL}")
endif ()

if (CMAKE_C_FLAGS_DEBUG MATCHES "/W[0-4]")
     string(REGEX REPLACE "/W[0-4]" "${VS_WARNING_LEVEL}" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
else ()
     set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${VS_WARNING_LEVEL}")
endif ()

set(CMAKE_CONFIGURATION_TYPES Debug Release)

if(WIN32)
    add_definitions(-DWIN32)

    if(${CMAKE_BUILD_TYPE} MATCHES "debug")
        set(JXIOT_BASE_LIB "pgcs_wind")
    else(${CMAKE_BUILD_TYPE} MATCHES "debug")
        set(JXIOT_BASE_LIB "pgcs_win")
    endif(${CMAKE_BUILD_TYPE} MATCHES "debug")
        set(JXIOT_BASE_LIB "pgcs_win")
else(WIN32)
    if(${CMAKE_BUILD_TYPE} MATCHES "debug")
        set(JXIOT_BASE_LIB "pgcs_unixd")
    else(${CMAKE_BUILD_TYPE} MATCHES "debug")
        set(JXIOT_BASE_LIB "pgcs_unix")
    endif(${CMAKE_BUILD_TYPE} MATCHES "debug")
endif(WIN32)

#Path to the include directory
set(source_dir src)

#Include custom CMake macro
include(macro)

