cmake_minimum_required(VERSION 2.8.5)
project(libdcpu-ci-objfmt)

add_library(libdcpu-ci-objfmt STATIC
	ldata.c
	ldata.h
	lprov.c
	lprov.h
	objfile.c
	objfile.h
)
include_directories(
	${bstring_SOURCE_DIR}
	${libdcpu_SOURCE_DIR}
)
