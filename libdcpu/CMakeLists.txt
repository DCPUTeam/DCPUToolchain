cmake_minimum_required(VERSION 2.8.5)
project(libdcpu)

include_directories(${bstring_SOURCE_DIR})
add_library(libdcpu STATIC
    bfile.h
    dcpu.h
    dirent.portable.h
    imap.h
    luaglob.h
    posix.h
    version.h
    bstring.h
    debug.h
    iio.h
    lexfix.h
    osutil.h
    rparse.h
    bfile.c
    bstring.c
    debug.c
    iio.c
    imap.c
    osutil.c
    rparse.c
    version.c
)
