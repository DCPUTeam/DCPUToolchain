cmake_minimum_required(VERSION 2.8.5)
project(ltestdcpu-pp)

bison_target(ltestdcpu-pp-parser parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.c COMPILE_FLAGS "-d -y")
flex_target(ltestdcpu-pp-lexer lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.c COMPILE_FLAGS \"--header-file=${CMAKE_CURRENT_BINARY_DIR}/lexer.h\")
add_flex_bison_dependency(ltestdcpu-pp-lexer ltestdcpu-pp-parser)
if(NOT MSVC)
    set_source_files_properties(${FLEX_ltestdcpu-pp-lexer_OUTPUTS} ${BISON_ltestdcpu-pp-parser_OUTPUTS} PROPERTIES COMPILE_FLAGS -Wno-implicit-function-declaration)
endif()

add_executable(ltestdcpu-pp
    main.c
    ${FLEX_ltestdcpu-pp-lexer_OUTPUTS}
    ${BISON_ltestdcpu-pp-parser_OUTPUTS}
    parser.y
    lexer.l
)
include_directories(
    ${ltestdcpu-pp_BINARY_DIR}
    ${argtable2_SOURCE_DIR}
    ${bstring_SOURCE_DIR}
    ${libdcpu_SOURCE_DIR}
    ${libdcpu-pp_SOURCE_DIR}
)
target_link_libraries(ltestdcpu-pp
    argtable2
    bstring
    libdcpu
    libdcpu-pp
)

# Define folder that holds test data.
set(BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../data/dcpu-pp")

enable_testing()
add_test(asm-line-01 ${CMAKE_CURRENT_BINARY_DIR}/ltestdcpu-pp asm ${BASE_DIR}/asm/line-01.pp)
add_test(asm-line-02 ${CMAKE_CURRENT_BINARY_DIR}/ltestdcpu-pp asm ${BASE_DIR}/asm/line-02.pp)
add_test(asm-line-03 ${CMAKE_CURRENT_BINARY_DIR}/ltestdcpu-pp asm ${BASE_DIR}/asm/line-03.pp)
add_test(asm-include-01 ${CMAKE_CURRENT_BINARY_DIR}/ltestdcpu-pp asm ${BASE_DIR}/asm/include-01.pp)
add_test(asm-include-02 ${CMAKE_CURRENT_BINARY_DIR}/ltestdcpu-pp asm ${BASE_DIR}/asm/include-02.pp)
add_test(asm-include-03 ${CMAKE_CURRENT_BINARY_DIR}/ltestdcpu-pp -f asm ${BASE_DIR}/asm/include-03.pp)
add_test(c-line-01 ${CMAKE_CURRENT_BINARY_DIR}/ltestdcpu-pp c ${BASE_DIR}/c/line-01.pp)
add_test(c-line-02 ${CMAKE_CURRENT_BINARY_DIR}/ltestdcpu-pp c ${BASE_DIR}/c/line-02.pp)
add_test(c-line-03 ${CMAKE_CURRENT_BINARY_DIR}/ltestdcpu-pp c ${BASE_DIR}/c/line-03.pp)
add_test(c-include-01 ${CMAKE_CURRENT_BINARY_DIR}/ltestdcpu-pp c ${BASE_DIR}/c/include-01.pp)
add_test(c-include-02 ${CMAKE_CURRENT_BINARY_DIR}/ltestdcpu-pp c ${BASE_DIR}/c/include-02.pp)
add_test(c-include-03 ${CMAKE_CURRENT_BINARY_DIR}/ltestdcpu-pp -f c ${BASE_DIR}/c/include-03.pp)
