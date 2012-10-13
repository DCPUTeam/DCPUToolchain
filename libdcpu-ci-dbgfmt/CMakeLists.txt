cmake_minimum_required(VERSION 2.8.5)
project(libdcpu-ci-dbgfmt)

add_library(libdcpu-ci-dbgfmt STATIC
    ddata.c
    ddata.h
)
include_directories(
    ${bstring_SOURCE_DIR}
    ${libdcpu_SOURCE_DIR}
    ${simclist_SOURCE_DIR}
)
cotire(libdcpu-ci-dbgfmt)
