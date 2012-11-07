/**

    File:       pp.h

    Project:    DCPU-16 Tools
    Component:  LibDCPU-pp

    Authors:    James Rhodes

    Description:    Declares the public API for using the preprocessor
                    inline in programs.

**/

#ifndef __DCPU_LIBDCPUPP_IMPL_H
#define __DCPU_LIBDCPUPP_IMPL_H

#include "pp.h"
#include <bstring.h>
#include <simclist.h>
#include <ppexpr.h>

///
/// Defines a preprocessor scope.
///
typedef struct
{
    bool active;
    size_t start_index;
    list_t old_handlers;
    list_t new_handlers;
} scope_t;

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
    unsigned int current_line;
    bstring current_filename;
    bstring default_filename;
    bool in_single_string;
    bool in_double_string;
    list_t scopes;
    size_t print_index; ///< Tracks where the printing cursor is for printf so successive calls appear correctly.
} state_t;

///
/// Defines a match structure.
///
struct __match
{
    freed_bstring text;
    void(*handler)(state_t* state, struct __match* match, bool* reprocess);
    void* userdata;
    bool line_start_only; ///< Whether this match can only be made at the start of a line (ignoring whitespace).
    bool identifier_only; ///< Whether this match is treated as an identifier.
    bool case_insensitive; ///< Whether this match should be made case insensitively.
};
typedef void(*match_handler_t)(state_t* state, struct __match* match, bool* reprocess);
typedef struct __match match_t;

///
/// Defines a parameter passed to a preprocessor directive.
///
typedef struct
{
    int type;
    int number;
    bstring string;
    struct expr* expr;
    list_t list;
} parameter_t;

bool ppimpl_has_input(state_t* state);
char ppimpl_get_input(state_t* state);
unsigned int ppimpl_get_current_line(state_t* state);
bstring ppimpl_get_current_filename(state_t* state);
char* ppimpl_get_location(state_t* state);
void ppimpl_printf(state_t* state, const char* msg, ...);
bool ppimpl_isolates(char c, bool at_start);
void ppimpl_push_scope(state_t* state, bool active);
void ppimpl_flip_scope(state_t* state);
void ppimpl_pop_scope(state_t* state);
void ppimpl_register(state_t* state, match_t* match);

#endif
