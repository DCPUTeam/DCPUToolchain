cmake_minimum_required(VERSION 2.8.5)
project(libdcpu-ci-objfmt)

add_library(libdcpu-ci-objfmt ${LIBRARY_FORMAT}
    ldata.c
    ldata.h
    lprov.c
    lprov.h
    objfile.c
    objfile.h
)
include_directories(
    ${bstring_SOURCE_DIR}
    ${libdcpu_SOURCE_DIR}
)
set_target_properties(libdcpu-ci-objfmt PROPERTIES PREFIX "")
cotire(libdcpu-ci-objfmt)

if(${DEPLOYABLE})
    install(TARGETS libdcpu-ci-objfmt
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
    file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
    install(FILES ${headers} DESTINATION include/dcpu/ci/objfmt)
endif()
