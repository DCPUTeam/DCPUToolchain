cmake_minimum_required(VERSION 2.8.5)
project(dtemu)

add_library(dtemu STATIC
	main.c
)
include_directories(
	${lua_SOURCE_DIR}
	${bstring_SOURCE_DIR}
	${simclist_SOURCE_DIR}
	${argtable2_SOURCE_DIR}
	${libdcpu_SOURCE_DIR}
	${libdcpu-vm_SOURCE_DIR}
	${libdcpu-ci-objfmt_SOURCE_DIR}
	${libdcpu-pp-expr_SOURCE_DIR}
	${GLFW_SOURCE_DIR}/include
	${png_SOURCE_DIR}
)
target_link_libraries(dtemu
	lua
	bstring
	simclist
	argtable2
	libdcpu
	libdcpu-vm
	libdcpu-ci-objfmt
	libdcpu-pp-expr
	glfw
	png
	z
)

