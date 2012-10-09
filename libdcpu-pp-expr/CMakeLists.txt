cmake_minimum_required(VERSION 2.8.5)
project(libdcpu-pp-expr)

add_library(libdcpu-pp-expr STATIC
	ppexpr.c
	ppexprlua.c
	ppexpr.h
	ppexprlua.h
	parser.y
	lexer.l
)
include_directories(
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
