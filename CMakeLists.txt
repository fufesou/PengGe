cmake_minimum_required(VERSION 3.2.0 FATAL_ERROR)
project(jxiot)

set(JXIOT_VERSION_MAJOR 0)
set(JXIOT_VERSION_MINOR 1)

set(JXIOT_NAME "jxiot")
set(JXIOT_VERSION "${JXIOT_VERSION_MAJOR}.${JXIOT_VERSION_MINOR}")
set(JXIOT_BRIEF "${JXIOT_NAME} is a simple communication utility project for users.")
set(PROJECT_ROOT_DIR "${PROJECT_SOURCE_DIR}")

add_definitions(-DCMAKE_PRO)

option(JXIOT_TEST_ENABLED "enable tests build" ON)

if(NOT COMMAND if_defined_set)
    set(cmake_directory ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
endif()

set(CMAKE_MODULE_PATH ${cmake_directory})
include(build_config)

add_subdirectory(src)
if(JXIOT_TEST_ENABLED)
    add_subdirectory(samples)
endif()
add_subdirectory(gui)

set(README_FILE ${PROJECT_SOURCE_DIR}/README.md)

if(NOT DOC_DISABLED)
    add_subdirectory(doc)
endif()

install(FILES
    ${README_FILE}
    #   ${LICENSE_FILE}
    DESTINATION ./bin
    )
