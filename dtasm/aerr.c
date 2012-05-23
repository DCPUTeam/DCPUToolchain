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
#include "aerr.h"

// Error strings
const char* err_strings[20] =
{
	"assembler: generic assembling error.\n",
	"assembler: label '%s' not found.\n",
	"assembler: attempted to output NULL operation.\n",
	"assembler: can not use register '%s' in brackets.\n",
	"assembler: can not use register '%s' as next value.\n",
	"assembler: relocation table too large.\n",
	"assembler: unsupported parameter passed to DAT instruction.\n",
	"assembler: unsupported parameter passed to non-DAT instruction.\n",
	"assembler: unsupported keyword encountered.\n",
	"assembler: extension table too large.\n",
	"assembler: extension %s is not known to the assembler.\n",
	"assembler: can not use label '%s' as next value.\n",
	"assembler: unrecognized opcode '%s' encountered.\n",
	"assembler: unable to include binary file '%s' inline.\n",
	"assembler: the .EXPORT and .IMPORT directives can not be used unless generating intermediate code.\n",
	"assembler: operation not defined for label resolution.\n",
	"assembler: unable to resolve '%s' as label resolution is not permitted at this time.\n",
	"assembler: the imported label '%s' may not be used as a component of an expression.\n",
	"assembler: expression '%s' evaluates to zero while being used as a divisor.\n",
	"assembler: .OUTPUT is not permitted prior to .SECTION.\n"
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
