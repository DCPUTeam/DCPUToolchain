cmake_minimum_required(VERSION 2.8.5)
project(libdcpu-ci-dbgfmt)

add_library(libdcpu-ci-dbgfmt ${LIBRARY_FORMAT}
    ddata.c
    ddata.h
)
include_directories(
    ${bstring_SOURCE_DIR}
    ${libdcpu_SOURCE_DIR}
    ${simclist_SOURCE_DIR}
)
set_target_properties(libdcpu-ci-dbgfmt PROPERTIES PREFIX "")
cotire(libdcpu-ci-dbgfmt)

if(${DEPLOYABLE})
    install(TARGETS libdcpu-ci-dbgfmt
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
    file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
    install(FILES ${headers} DESTINATION include/dcpu/ci/dbgfmt)
endif()
