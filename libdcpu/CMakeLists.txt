cmake_minimum_required(VERSION 2.8.5)
project(libdcpu)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/derr.defs.h ${CMAKE_CURRENT_SOURCE_DIR}/derr.defs.c
    COMMAND tool-errgen ${CMAKE_CURRENT_SOURCE_DIR}/err.d ${CMAKE_CURRENT_SOURCE_DIR}/derr.defs.h ${CMAKE_CURRENT_SOURCE_DIR}/derr.defs.c
    COMMENT "Generating error definition files..."
    )
include_directories(
    ${bstring_SOURCE_DIR}
    ${argtable2_SOURCE_DIR}
    ${libdcpu_SOURCE_DIR}
    # Need to manual path this since we have a cyclic dependency.
    ${toolchain_SOURCE_DIR}/libdcpu-pp-expr
)
add_library(libdcpu STATIC
    derr.defs.c
    derr.defs.h
    derr.c
    derr.h
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
cotire(libdcpu)
