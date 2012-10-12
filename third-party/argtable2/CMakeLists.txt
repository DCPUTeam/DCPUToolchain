cmake_minimum_required(VERSION 2.8.5)
project(argtable2)

add_library(argtable2 STATIC
    arg_dbl.c
    arg_end.c
    arg_file.c
    arg_int.c
    arg_lit.c
    arg_rem.c
    arg_str.c
    argtable2.c
    getopt.c
    getopt1.c
    argtable2.h
    getopt.h
)
add_definitions("-DSTDC_HEADERS -DHAVE_STDLIB_H -DHAVE_STRING_H")
set_property(TARGET argtable2 PROPERTY FOLDER "third-party")
cotire(argtable2)
