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
/// @brief The callback when the preprocessor checks to see if there is more input.
///
/// @param userdata The userdata associated with the preprocessor state.
/// @return Whether there is another character available.
///
typedef bool(*has_t)(void* userdata);

///
/// @brief The callback when the preprocessor wants to recieve another character.
///
/// @param userdata The userdata associated with the preprocessor state.
/// @return The next character in the input.
///
typedef char(*pop_t)(void* userdata);

///
/// @brief The callback when the preprocessor is outputting a result character.
///
/// @param userdata The userdata associated with the preprocessor state.
/// @param char The character the preprocessor wants to output.
///
typedef void(*push_t)(void* userdata, char value);

void ppimpl(freed_bstring filename, int line, freed_bstring lang, has_t has_input, pop_t input, push_t output, void* userdata);

#endif
