/**

    File:       pp.h

    Project:    DCPU-16 Tools
    Component:  LibDCPU-pp

    Authors:    James Rhodes

    Description:    Declares the public API for using the preprocessor
                    inline in programs.

**/

#ifndef __DCPU_LIBDCPUPP_H
#define __DCPU_LIBDCPUPP_H

#include <dcpu.h>
#include <bstring.h>

///
/// @brief Defines the character type that holds the value and source location.
///
typedef struct
{
    char value;
    bstring file;
    int column;
    int line;
} char_t;

typedef bool (*has_t)();
typedef char_t (*pop_t)();
typedef void (*push_t)(char_t);

void ppimpl(freed_bstring filename, int line, freed_bstring lang, has_t has_input, pop_t input, push_t output);

#endif
