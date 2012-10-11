cmake_minimum_required(VERSION 2.8.5)
project(dtdb)

find_package(OpenGL REQUIRED)
add_executable(dtdb
	main.c
	packets.c
	packets.h
	sdp.c
	sdp.h	
)
include_directories(
	${readline_SOURCE_DIR}
	${pthread_SOURCE_DIR}
	${argtable2_SOURCE_DIR}
	${simclist_SOURCE_DIR}
	${bstring_SOURCE_DIR}
	${lua_SOURCE_DIR}
	${libdcpu-pp-expr_SOURCE_DIR}
	${libdcpu-ci-dbgfmt_SOURCE_DIR}
	${libdcpu-vm-dbg_SOURCE_DIR}
	${libdcpu-vm-dbg_BINARY_DIR}
	${libdcpu-vm_SOURCE_DIR}
	${libdcpu_SOURCE_DIR}
	${FLEX_UNISTD_INCLUDE}
	${GLFW_SOURCE_DIR}/include
	${png_SOURCE_DIR}
)
target_link_libraries(dtdb
	${readline_LIBRARY}
	${pthread_LIBRARY}
	argtable2
	simclist
	bstring
	lua
	libdcpu-pp-expr
	libdcpu-ci-dbgfmt
	libdcpu-vm-dbg
	libdcpu-vm
	libdcpu
	glfw
	png
	z
	${OPENGL_LIBRARIES}
	${GLFW_LIBRARIES}
)

