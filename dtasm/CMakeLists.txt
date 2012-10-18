cmake_minimum_required(VERSION 2.8.5)
project(dtasm)

add_executable(dtasm
    main.c
)
include_directories(
    ${simclist_SOURCE_DIR}
    ${argtable2_SOURCE_DIR}
    ${bstring_SOURCE_DIR}
    ${libdcpu_SOURCE_DIR}
    ${libdcpu-ci-asm_SOURCE_DIR}
    ${libdcpu-ci-objfmt_SOURCE_DIR}
    ${libdcpu-pp_SOURCE_DIR}
    ${libdcpu-pp-expr_SOURCE_DIR}
    ${libdcpu-ci-objfmt_SOURCE_DIR}
    ${libdcpu-ci-dbgfmt_SOURCE_DIR}
)
target_link_libraries(dtasm
    bstring
    argtable2
    libdcpu-ci-asm
)
cotire(dtasm)
