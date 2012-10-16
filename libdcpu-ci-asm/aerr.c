/**

	File:		aerr.c

	Project:	DCPU-16 Tools
	Component:	Assembler

	Authors:	James Rhodes

	Description:	Implements error handling infrastructure
			so that the assembler can safely handle
			errors as they occur.

**/

#include <stdlib.h>
#include <setjmp.h>
#include <ppexpr.h>
#include <debug.h>
#include "aerr.h"

// Error strings
const char* err_strings[_ERR_COUNT] =
{
	"generic assembling error.\n",
	"label '%s' not found.\n",
	"attempted to output NULL operation.\n",
	"can not use register '%s' in brackets.\n",
	"can not use register '%s' as next value.\n",

	"relocation table too large.\n",
	"unsupported parameter passed to DAT instruction.\n",
	"unsupported parameter passed to non-DAT instruction.\n",
	"unsupported keyword encountered.\n",
	"extension table too large.\n",

	"extension %s is not known to the assembler.\n",
	"can not use label '%s' as next value.\n",
	"unrecognized opcode '%s' encountered.\n",
	"unable to include binary file '%s' inline.\n",
	"the .EXPORT and .IMPORT directives can not be used unless generating intermediate code.\n",

	"operation not defined for label resolution.\n",
	"unable to resolve '%s' as label resolution is not permitted at this time.\n",
	"the imported label '%s' may not be used as a component of an expression.\n",
	"expression '%s' evaluates to zero while being used as a divisor.\n",
	".OUTPUT is not permitted prior to .SECTION.\n",

	"invalid parameter count for instruction.\n",

    // Warnings
	"expressions will not be adjusted at link or relocation time. ensure labels are not used as part of expressions.\n",
	".ORIGIN should not be used for relocation as it is not portable between kernels.\n",
	"relative adjustments to PC (such as SUB PC, <n>) are not safe due to literal shortening.\n"
};

// Warning policies
struct warnpolicy warnpolicies[_WARN_COUNT] =
{
	{ WARN_EXPRESSION_NOT_ADJUSTED, "expression-not-adjusted", false, false },
	{ WARN_USE_RELOCATION_NOT_ORIGIN, "origin-usage", false, false },
	{ WARN_RELATIVE_PC_ADDRESSING, "relative-jump", false, false },
};

// Error definition
jmp_buf errjmp;

// Utility method for throwing errors.
void ahalt(int errid, const char* errdata)
{
	struct errinfo* err = malloc(sizeof(struct errinfo));
	err->errid = errid;
	err->errdata = errdata;
	longjmp(errjmp, (long)err);
}

void awarn(int errid, const char* errdata)
{
	const char* prepend = "warning: ";
	int msglen;
	char* msg;

	if (warnpolicies[errid - _WARN_OFFSET].errid == errid &&
		warnpolicies[errid - _WARN_OFFSET].treat_as_error)
		ahalt(errid, errdata);
	if (warnpolicies[errid - _WARN_OFFSET].errid == errid &&
		!warnpolicies[errid - _WARN_OFFSET].silence)
	{
		// FIXME: Use bstrings here.
		msglen = strlen(err_strings[errid]) + strlen(prepend) + 1;
		msg = malloc(msglen);
		memset(msg, '\0', msglen);
		strcat(msg, prepend);
		strcat(msg, err_strings[errid]);
		printd(LEVEL_WARNING, msg, errdata);
	}
}

uint16_t ahalt_label_resolution_not_permitted(bstring name)
{
	ahalt(ERR_LABEL_RESOLUTION_NOT_PERMITTED, name->data);
	return 0;
}

void ahalt_expression_exit_handler(int code, void* data)
{
	switch (code)
	{
		case EXPR_EXIT_LABEL_NOT_FOUND:
			ahalt(ERR_LABEL_NOT_FOUND, ((bstring)data)->data);
		case EXPR_EXIT_DIVIDE_BY_ZERO:
			ahalt(ERR_EXPRESSION_DIVIDE_BY_ZERO, ((bstring)data)->data);
		default:
			ahalt(ERR_GENERIC, NULL);
	}
}
