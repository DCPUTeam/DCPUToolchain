cmake_minimum_required(VERSION 2.8.5)
project(dtpp)

add_library(dtpp STATIC
	main.c
)
include_directories(
	${argtable2_SOURCE_DIR}
	${simclist_SOURCE_DIR}
	${bstring_SOURCE_DIR}
	${libdcpu_SOURCE_DIR}
	${libdcpu-pp_SOURCE_DIR}
	${FLEX_UNISTD_INCLUDE}
)
target_link_libraries(dtpp
	argtable2
	bstring
	simclist
	libdcpu
	libdcpu-pp
)
