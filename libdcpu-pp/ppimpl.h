/**

    File:       pp.h
    
    Project:    DCPU-16 Tools
    Component:  LibDCPU-pp

    Authors:    James Rhodes

    Description:    Declares the public API for using the preprocessor
                    inline in programs.

**/

#ifndef __DCPU_LIBDCPU_IMPL_H
#define __DCPU_LIBDCPU_IMPL_H

#include <dcpu.h>
#include <simclist.h>

typedef bool(*has_t)();
typedef char(*pop_t)();
typedef void(*push_t)(char);

void ppimpl(has_t has_input, pop_t input, push_t output);

#ifdef PREPROCESSOR_INTERNALS

///
/// Defines the preprocessor state.
///
typedef struct
{
    list_t cached_input;
    list_t cached_output;
    has_t has_input;
    pop_t input;
    push_t output;
    list_t handlers;    
} state_t;

///
/// Defines a match structure.
///
struct __match
{
    freed_bstring text;
    void(*handler)(state_t* state, struct __match* match, bool* reprocess);
    bool line_start_only; //< Whether this match can only be made at the start of a line (ignoring whitespace).
};
typedef void(*match_handler_t)(state_t* state, struct __match* match, bool* reprocess);
typedef struct __match match_t;

char ppimpl_get_input(state_t* state);

#endif

#endif
