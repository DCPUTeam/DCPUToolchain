/**

    File:       aerr.h

    Project:    DCPU-16 Tools
    Component:  Assembler

    Authors:    James Rhodes

    Description:    Declares error codes and functions in the
            error handling infrastructure.

**/

#ifndef __DCPU_ASM_AERR_H
#define __DCPU_ASM_AERR_H

#include <stdint.h>
#include <setjmp.h>
#include "dcpu.h"
#include "bstring.h"

// Structure for holding warning policy information
struct warnpolicy
{
    int errid;
    const char* name;
    bool treat_as_error; //< Whether the warning should be treated as an error.
    bool silence; //< Whether the warning should be silenced.
};

// Error codes
#include "derr.defs.h"

// Variables
extern jmp_buf __derrjmp;
extern bool __derrset;
#ifndef INTERNAL_DERR_REFERENCING
extern const char* err_strings[];
extern struct warnpolicy warnpolicies[];
#endif

// Structure for holding error information
struct errinfo
{
    int errid; ///< The error ID.
    const char* errdata; ///< Additional error information.
    bool has_location; ///< Whether this error contains location information.
    unsigned int line; ///< The line number the error occurred on (in the source file).
    const char* filename; ///< The filename of the source file that caused the error.
};

// Utility method for throwing errors.
#define dsethalt() ((__derrset = true), setjmp(__derrjmp))
struct errinfo* derrinfo();
void dhalt(int errid, const char* errdata);
void dhaltloc(int errid, const char* errdata, unsigned int line, const char* filename);
void dwarn(int errid, const char* errdata);
void dwarnloc(int errid, const char* errdata, unsigned int line, const char* filename);
void dautohandle();
#ifdef ARGTABLE2
// Only show this function if we're including
// argtable2 anyway.
void dsetwarnpolicy(struct arg_str* warning_policies);
#endif
uint16_t dhalt_label_resolution_not_permitted(bstring name);
void dhalt_expression_exit_handler(int code, void* data);

#endif
