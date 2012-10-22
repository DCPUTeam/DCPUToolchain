cmake_minimum_required(VERSION 2.8.5)
project(libdcpu-pp)

bison_target(libdcpu-pp-param-parser ppparamparser.y ${CMAKE_CURRENT_BINARY_DIR}/ppparamparser.c COMPILE_FLAGS "-d -y")
flex_target(libdcpu-pp-param-lexer ppparamlexer.l ${CMAKE_CURRENT_BINARY_DIR}/ppparamlexer.c COMPILE_FLAGS \"--header-file=${CMAKE_CURRENT_BINARY_DIR}/ppparamlexer.h\")
add_flex_bison_dependency(libdcpu-pp-param-lexer libdcpu-pp-param-parser)
if(NOT MSVC)
    set_source_files_properties(${FLEX_libdcpu-pp-param-lexer_OUTPUTS} ${BISON_libdcpu-pp-param-parser_OUTPUTS} PROPERTIES COMPILE_FLAGS -Wno-implicit-function-declaration)
endif()
add_library(libdcpu-pp STATIC
    pp.h
    ppimpl.h
    ppimpl.c
    ppparam.h
    ppparamlexer.l
    ppparamparser.y
    ${FLEX_libdcpu-pp-param-lexer_OUTPUTS}
    ${BISON_libdcpu-pp-param-parser_OUTPUTS}
    directives/asm.h
    directives/asm/line.c
    directives/asm/include.c
    directives/asm/define.c
    directives/asm/macro.c
)
auto_group(libdcpu-pp)
include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${lua_SOURCE_DIR}
    ${bstring_SOURCE_DIR}
    ${simclist_SOURCE_DIR}
    ${libdcpu_SOURCE_DIR}
    ${libdcpu-pp-expr_SOURCE_DIR}
    ${FLEX_UNISTD_INCLUDE}
)
target_link_libraries(libdcpu-pp
    lua
    bstring
    simclist
    libdcpu
    libdcpu-pp-expr
)
cotire(libdcpu-pp)
