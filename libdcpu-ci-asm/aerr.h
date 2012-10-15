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
#include <bstring.h>
#include <ppexpr.h>

// Error codes
#define ERR_GENERIC 0
#define ERR_LABEL_NOT_FOUND 1
#define ERR_OUTPUT_NULL 2
#define ERR_BRACKETED_REGISTER_UNSUPPORTED 3
#define ERR_NEXTED_REGISTER_UNSUPPORTED 4
#define ERR_RELOCATION_TABLE_TOO_LARGE 5
#define ERR_DAT_UNSUPPORTED_PARAMETER 6
#define ERR_GEN_UNSUPPORTED_PARAMETER 7
#define ERR_UNSUPPORTED_KEYWORD 8
#define ERR_EXTENSION_TABLE_TOO_LARGE 9
#define ERR_EXTENSION_UNKNOWN 10
#define ERR_NEXTED_LABEL_UNSUPPORTED 11
#define ERR_UNKNOWN_OPCODE 12
#define ERR_UNABLE_TO_INCBIN 13
#define ERR_NOT_GENERATING_INTERMEDIATE_CODE 14
#define ERR_OPERATION_NOT_DEFINED_FOR_LABEL_RESOLUTION 15
#define ERR_LABEL_RESOLUTION_NOT_PERMITTED 16
#define ERR_EXPRESSION_NOT_PERMITTED 17
#define ERR_EXPRESSION_DIVIDE_BY_ZERO 18
#define ERR_OUTPUT_BEFORE_SECTION 19
#define ERR_INVALID_PARAMETER_COUNT 20
// Warning codes (note that warnings MUST follow
// all error codes)
#define WARN_EXPRESSION_NOT_ADJUSTED 21
#define WARN_USE_RELOCATION_NOT_ORIGIN 22
#define WARN_RELATIVE_PC_ADDRESSING 23

// Error value information
#define _WARN_OFFSET 21
#define _ERR_COUNT 24
#define _WARN_COUNT (_ERR_COUNT - _WARN_OFFSET)

// Structure for holding warning policy information
struct warnpolicy
{
	int errid;
	const char* name;
	bool treat_as_error; //< Whether the warning should be treated as an error.
	bool silence; //< Whether the warning should be silenced.
};

// Variables
extern const char* err_strings[];
extern jmp_buf errjmp;
extern struct warnpolicy warnpolicies[];

// Structure for holding error information
struct errinfo
{
	int errid;
	const char* errdata;
};

// Utility method for throwing errors.
void ahalt(int errid, const char* errdata);
void awarn(int errid, const char* errdata);
uint16_t ahalt_label_resolution_not_permitted(bstring name);
void ahalt_expression_exit_handler(int code, void* data);

#endif
