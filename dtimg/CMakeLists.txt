cmake_minimum_required(VERSION 2.8.5)
project(dtimg)

add_executable(dtimg
	base.h
	main.c
	micro.c
	micro.h
)
include_directories(
	${bstring_SOURCE_DIR}
	${simclist_SOURCE_DIR}
	${argtable2_SOURCE_DIR}
	${libdcpu_SOURCE_DIR}
)
target_link_libraries(dtimg
	argtable2
	bstring
	simclist
	libdcpu
)

