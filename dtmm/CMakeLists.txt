cmake_minimum_required(VERSION 2.8.5)
project(dtmm)

add_executable(dtmm 
	main.c
)
include_directories(
	${bstring_SOURCE_DIR}
	${simclist_SOURCE_DIR}
	${argtable2_SOURCE_DIR}
	${libdcpu_SOURCE_DIR}
	${toolchain_SOURCE_DIR}/third-party/curl/include
)
target_link_libraries(dtmm
	argtable2
	bstring
	simclist
	lua
	${toolchain_SOURCE_DIR}/third-party/curl/lib/libcurl.lib
	libdcpu
)

