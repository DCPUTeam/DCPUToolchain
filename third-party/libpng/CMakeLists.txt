cmake_minimum_required(VERSION 2.8.5)
project(png)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/pnglibconf.h.prebuilt
               ${CMAKE_CURRENT_BINARY_DIR}/pnglibconf.h)
add_library(png STATIC
	png.c
	pngerror.c
	pngget.c
	pngmem.c
	pngpread.c
	pngread.c
	pngrio.c
	pngrtran.c
	pngrutil.c
	pngset.c
	pngtrans.c
	pngwio.c
	pngwrite.c
	pngwtran.c
	pngwutil.c
	png.h
	pngconf.h
	pngdebug.h
	pnginfo.h
	pngstruct.h
)
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${z_SOURCE_DIR})
add_dependencies(png z)
set_property(TARGET png PROPERTY FOLDER "third-party")
