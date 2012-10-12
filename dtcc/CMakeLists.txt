cmake_minimum_required(VERSION 2.8.5)
project(dtcc)

add_executable(dtcc
    main.cpp
)
include_directories(
    ${bstring_SOURCE_DIR}
    ${argtable2_SOURCE_DIR}
    ${libdcpu_SOURCE_DIR}
    ${libdcpu-pp_SOURCE_DIR}
    ${libdcpu-ci-lang_SOURCE_DIR}
    ${libdcpu-ci-lang-c_SOURCE_DIR}
    ${FLEX_UNISTD_INCLUDE}
)
target_link_libraries(dtcc
    argtable2
    bstring
    libdcpu
    libdcpu-pp
    libdcpu-ci-lang-c
    libdcpu-ci-lang
)
cotire(dtcc)
