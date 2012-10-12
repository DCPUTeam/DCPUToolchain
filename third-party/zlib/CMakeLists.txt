cmake_minimum_required(VERSION 2.8.5)
project(z)

if(NOT MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error -Wno-implicit-function-declaration")
endif()
add_library(z STATIC
    adler32.c
    compress.c
    crc32.c
    deflate.c
    gzclose.c
    gzlib.c
    gzread.c
    gzwrite.c
    infback.c
    inffast.c
    inftrees.c
    inflate.c
    trees.c
    uncompr.c
    zutil.c
    crc32.h
    deflate.h
    gzguts.h
    inffast.h
    inffixed.h
    inflate.h
    inftrees.h
    inflate.h
    trees.h
    zconf.h
    zlib.h
    zutil.h
)
set_property(TARGET z PROPERTY FOLDER "third-party")
