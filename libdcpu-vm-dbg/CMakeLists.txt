cmake_minimum_required(VERSION 2.8.5)
project(libdcpu-vm-dbg)

bison_target(parser parser.y parser.c COMPILE_FLAGS "-d -y")
flex_target(lexer lexer.l lexer.c COMPILE_FLAGS "--header-file=lexer.h")
add_flex_bison_dependency(lexer parser)
add_library(libdcpu-vm-dbg STATIC
	backtrace.c
	breakpoint.c
	dbgaux.c
	dbglua.c
	backtrace.h
	breakpoint.h
	dbgaux.h
	dbglua.h
	parser.y
	lexer.l
	${CMAKE_CURRENT_SOURCE_DIR}/parser.c
	${CMAKE_CURRENT_SOURCE_DIR}/lexer.c
)
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}	
	${bstring_SOURCE_DIR}
	${simclist_SOURCE_DIR}
	${lua_SOURCE_DIR}
	${libdcpu_SOURCE_DIR}
	${libdcpu-vm_SOURCE_DIR}
	${libdcpu-ci-dbgfmt_SOURCE_DIR}
	${libdcpu-pp-expr_SOURCE_DIR}
	${GLFW_SOURCE_DIR}/include
	${FLEX_UNISTD_INCLUDE}
)
target_link_libraries(libdcpu-vm-dbg
	bstring
	simclist
	lua
	libdcpu
	libdcpu-vm
	libdcpu-ci-dbgfmt
	libdcpu-pp-expr
)

