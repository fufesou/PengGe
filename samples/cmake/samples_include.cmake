file(GLOB PRO_FILES ${CMAKE_CURRENT_SOURCE_DIR} *.c *.h)

add_executable(${TARGET} ${PRO_FILES})
include_directories(
    ${PROJECT_ROOT_DIR}/src/include
    )

if(WIN32)
set(EXTRA_LIBS
    ${EXTRA_LIBS} ${JXIOT_BASE_LIB} wsock32 winmm ws2_32
    )
else(WIN32)
set(EXTRA_LIBS ${EXTRA_LIBS} ${JXIOT_BASE_LIB})
endif(WIN32)

target_link_libraries (${TARGET} ${EXTRA_LIBS})

if(MINGW OR UNIX)
    target_link_libraries(${TARGET} pthread)
endif()

