cmake_minimum_required(VERSION 2.8.5)
project(libdcpu-pp-expr)

bison_target(libdcpu-pp-expr-parser parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.c COMPILE_FLAGS "-d -y")
flex_target(libdcpu-pp-expr-lexer lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.c COMPILE_FLAGS \"--header-file=${CMAKE_CURRENT_BINARY_DIR}/lexer.h\")
add_flex_bison_dependency(libdcpu-pp-expr-lexer libdcpu-pp-expr-parser)
if(NOT MSVC)
    set_source_files_properties(${FLEX_libdcpu-pp-expr-lexer_OUTPUTS} ${BISON_libdcpu-pp-expr-parser_OUTPUTS} PROPERTIES COMPILE_FLAGS -Wno-implicit-function-declaration)
endif()
add_library(libdcpu-pp-expr ${LIBRARY_FORMAT}
    ppexpr.c
    ppexprlua.c
    ppexpr.h
    ppexprlua.h
    ${FLEX_libdcpu-pp-expr-lexer_OUTPUTS}
    ${BISON_libdcpu-pp-expr-parser_OUTPUTS}
    parser.y
    lexer.l
)
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${bstring_SOURCE_DIR}
    ${libdcpu_SOURCE_DIR}
    ${simclist_SOURCE_DIR}
    ${lua_SOURCE_DIR}
    ${FLEX_UNISTD_INCLUDE}
)
target_link_libraries(libdcpu-pp-expr
    lua
    simclist
)
set_target_properties(libdcpu-pp-expr PROPERTIES PREFIX "")
cotire(libdcpu-pp-expr)

if(${DEPLOYABLE})
    install(TARGETS libdcpu-pp-expr
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
    file(GLOB headers_gen "${CMAKE_CURRENT_BINARY_DIR}/*.h")
    file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
    install(FILES ${headers} ${headers_gen} DESTINATION include/dcpu/pp/expr)
endif()
