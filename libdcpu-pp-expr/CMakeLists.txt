cmake_minimum_required(VERSION 2.8.5)
project(libdcpu-pp-expr)

bison_target(pp-parser parser.y parser.c COMPILE_FLAGS "-d -y")
flex_target(pp-lexer lexer.l lexer.c COMPILE_FLAGS "--header-file=lexer.h")
add_flex_bison_dependency(pp-lexer pp-parser)
add_library(libdcpu-pp-expr STATIC
	ppexpr.c
	ppexprlua.c
	ppexpr.h
	ppexprlua.h
	parser.y
	lexer.l
	${CMAKE_CURRENT_SOURCE_DIR}/parser.c
	${CMAKE_CURRENT_SOURCE_DIR}/lexer.c
)
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${bstring_SOURCE_DIR}
	${libdcpu_SOURCE_DIR}
	${simclist_SOURCE_DIR}
	${lua_SOURCE_DIR}
	${FLEX_UNISTD_INCLUDE}
)
target_link_libraries(libdcpu-pp-expr
	lua
	bstring
	simclist
	libdcpu
)
