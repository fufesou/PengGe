cmake_minimum_required(VERSION 3.2.0 FATAL_ERROR)
project(base_lib)

set(JXIOT_CS_VERSION_MAJOR  0)
set(JXIOT_CS_VERSION_MINOR  1)

set(MAX_MSG_LEN           1024)
set(MAX_BUF_LEN           2048)
set(NUM_THREAD               8)
set(SERVER_POOL_NUM_ITEM    64)

set(ACCOUNT_PASSWD_LEN      32)
set(ACCOUNT_USERNAME_LEN    64)
set(ACCOUNT_TEL_LEN         16)
set(ACCOUNT_USERNUM_LEN     16)
set(ACCOUNT_PERFILE_NUM   1024)
set(ACCOUNT_FILE_NUM       100)

configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/include/config_macros.h.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/common/config_macros.h"
    )

file(GLOB_RECURSE CS_SRC_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.c)

add_library(${JXIOT_BASE_LIB} STATIC ${CS_SRC_FILES})

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
