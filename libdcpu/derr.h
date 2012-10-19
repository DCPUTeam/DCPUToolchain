/**

	File:		aerr.h

	Project:	DCPU-16 Tools
	Component:	Assembler

	Authors:	James Rhodes

	Description:	Declares error codes and functions in the
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
extern const char* err_strings[];
extern struct warnpolicy warnpolicies[];

// Structure for holding error information
struct errinfo
{
	int errid;
	const char* errdata;
};

// Utility method for throwing errors.
#define dsethalt() (setjmp(__derrjmp))
void dhalt(int errid, const char* errdata);
void dwarn(int errid, const char* errdata);
#ifdef ARGTABLE2
// Only show this function if we're including
// argtable2 anyway.
void dsetwarnpolicy(struct arg_str* warning_policies);
#endif
uint16_t dhalt_label_resolution_not_permitted(bstring name);
void dhalt_expression_exit_handler(int code, void* data);

#endif
