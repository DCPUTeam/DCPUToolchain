cmake_minimum_required(VERSION 2.8.5)
project(libdcpu-pp)

bison_target(libdcpu-pp-parser parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.c COMPILE_FLAGS "-d -y")
flex_target(libdcpu-pp-lexer lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.c COMPILE_FLAGS "--header-file=lexer.h")
add_flex_bison_dependency(libdcpu-pp-lexer libdcpu-pp-parser)
add_library(libdcpu-pp STATIC
	pp.h
	ppfind.h
	pplua.h
	pp.c
	ppfind.c
	pplua.c
	${FLEX_libdcpu-pp-lexer_OUTPUTS}
	${BISON_libdcpu-pp-parser_OUTPUTS}
	parser.y
	lexer.l
)
include_directories(
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
