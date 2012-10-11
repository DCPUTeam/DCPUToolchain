cmake_minimum_required(VERSION 2.8.5)
project(libdcpu-vm-dbg)

bison_target(libdcpu-vm-dbg-parser parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.c COMPILE_FLAGS "-d -y")
flex_target(libdcpu-vm-dbg-lexer lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.c COMPILE_FLAGS "--header-file=lexer.h")
add_flex_bison_dependency(libdcpu-vm-dbg-lexer libdcpu-vm-dbg-parser)
add_library(libdcpu-vm-dbg STATIC
	backtrace.c
	breakpoint.c
	dbgaux.c
	dbglua.c
	backtrace.h
	breakpoint.h
	dbgaux.h
	dbglua.h
	${FLEX_libdcpu-vm-dbg-lexer_OUTPUTS}
	${BISON_libdcpu-vm-dbg-parser_OUTPUTS}
	parser.y
	lexer.l
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
	libdcpu-ci-dbgfmt
	libdcpu-pp-expr
	simclist
	lua
	${X11_LIBRARIES}
)
