cmake_minimum_required(VERSION 2.8.5)
project(dtasm)

bison_target(dtasm-parser parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.c COMPILE_FLAGS "-d -y")
flex_target(dtasm-lexer lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.c COMPILE_FLAGS \"--header-file=${CMAKE_CURRENT_BINARY_DIR}/lexer.h\")
add_flex_bison_dependency(dtasm-lexer dtasm-parser)
if(NOT MSVC)
    set_source_files_properties(${FLEX_dtasm-lexer_OUTPUTS} ${BISON_dtasm-parser_OUTPUTS} PROPERTIES COMPILE_FLAGS -Wno-implicit-function-declaration)
endif()
add_executable(dtasm
    main.c
    aerr.c
    aerr.h
    aout.c
    aout.h
    assem.c
    assem.h
    main.c
    node.h
    textn.c
    textn.h
    treloc.c
    treloc.h
    ${FLEX_dtasm-lexer_OUTPUTS}
    ${BISON_dtasm-parser_OUTPUTS}
    parser.y
    lexer.l
)
include_directories(
    ${argtable2_SOURCE_DIR}
    ${simclist_SOURCE_DIR}
    ${bstring_SOURCE_DIR}
    ${libdcpu_SOURCE_DIR}
    ${libdcpu-pp_SOURCE_DIR}
    ${libdcpu-pp-expr_SOURCE_DIR}
    ${libdcpu-ci-objfmt_SOURCE_DIR}
    ${libdcpu-ci-dbgfmt_SOURCE_DIR}
    ${FLEX_UNISTD_INCLUDE}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)
target_link_libraries(dtasm
    argtable2
    bstring
    simclist
    libdcpu
    libdcpu-pp
    libdcpu-pp-expr
    libdcpu-ci-objfmt
    libdcpu-ci-dbgfmt
)
cotire(dtasm)
