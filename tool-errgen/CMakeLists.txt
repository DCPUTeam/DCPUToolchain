cmake_minimum_required(VERSION 2.8.5)
project(tool-errgen)

bison_target(tool-errgen-parser parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.c COMPILE_FLAGS "-d -y")
flex_target(tool-errgen-lexer lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.c COMPILE_FLAGS \"--header-file=${CMAKE_CURRENT_BINARY_DIR}/lexer.h\")
add_flex_bison_dependency(tool-errgen-lexer tool-errgen-parser)
if(NOT MSVC)
    set_source_files_properties(${FLEX_tool-errgen-lexer_OUTPUTS} ${BISON_tool-errgen-parser_OUTPUTS} PROPERTIES COMPILE_FLAGS -Wno-implicit-function-declaration)
endif()

add_executable(tool-errgen
    ${FLEX_tool-errgen-lexer_OUTPUTS}
    ${BISON_tool-errgen-parser_OUTPUTS}
    parser.y
    lexer.l
)
include_directories(
    ${FLEX_UNISTD_INCLUDE}
    ${simclist_SOURCE_DIR}
)
target_link_libraries(tool-errgen
    simclist
)
