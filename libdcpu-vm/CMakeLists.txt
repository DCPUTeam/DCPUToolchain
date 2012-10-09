cmake_minimum_required(VERSION 2.8.5)
project(libdcpu-vm)

add_library(libdcpu-vm STATIC
	dcpu.c
	dcpubase.h
	dcpudis.h
	dcpuhook.h
	dcpuops.h
	hw.h
	hwioascii.h
	hwkeyboard.h
	hwlem1802.h
	hwlem1802mem.h
	hwlem1802util.h
	hwlua.h
	hwluacpu.h
	hwm35fd.h
	hwsped3.h
	hwtimer.h
	timing.h
	dcpubase.c
	dcpudis.c
	dcpuhook.c
	dcpuops.c
	hw.c
	hwioascii.c
	hwkeyboard.c
	hwlem1802.c
	hwlem1802mem.c
	hwlem1802util.c
	hwlua.c
	hwluacpu.c
	hwm35fd.c
	hwsped3.c
	hwtimer.c
	timing.c
)
include_directories(
	${png_SOURCE_DIR}
	${png_BINARY_DIR}
	${z_SOURCE_DIR}
	${bstring_SOURCE_DIR}
	${libdcpu_SOURCE_DIR}
	${lua_SOURCE_DIR}
	${simclist_SOURCE_DIR}
	${libdcpu-pp-expr_SOURCE_DIR}
	${GLFW_SOURCE_DIR}/include
)
target_link_libraries(libdcpu-vm
	bstring
	lua
	libdcpu
	simclist
	libdcpu-pp-expr
	glfw
	png
	z
)

# Copy the default font to the output directory
# FIXME: file(COPY ./defaultfont.png DESTINATION "${CMAKE_CURRENT_OUTPUT_DIR}" USE_SOURCE_PERMISSIONS)

