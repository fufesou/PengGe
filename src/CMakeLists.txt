cmake_minimum_required(VERSION 3.2.0 FATAL_ERROR)
project(base_lib)

file(GLOB_RECURSE JXIOT_SRC_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.c)

add_library(${JXIOT_BASE_LIB} STATIC ${JXIOT_SRC_FILES})

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

install(TARGETS ${JXIOT_BASE_LIB}
    ARCHIVE DESTINATION lib/static
    COMPONENT libraries)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include
    DESTINATION ${PROJECT_SOURCE_DIR}/include
    COMPONENT headers
    )
