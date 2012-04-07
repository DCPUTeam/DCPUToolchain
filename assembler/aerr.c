/**

	File:			aerr.c

	Project:		DCPU-16 Tools
	Component:		Assembler

	Authors:		James Rhodes

	Description:	Implements error handling infrastructure
					so that the assembler can safely handle
					errors as they occur.

**/

#include <setjmp.h>
#include "aerr.h"

// Error strings
const char* err_strings[8] = 
{
	"assembler: generic assembling error.\n",
	"assembler: label '%s' not found.\n",
	"assembler: attempted to output NULL operation.\n",
	"assembler: can not use register '%s' in brackets.\n",
	"assembler: can not use register '%s' as next value.\n",
	"assembler: relocation table too large.\n",
	"assembler: unsupported parameter passed to DAT instruction.\n",
	"assembler: unsupported parameter passed to non-DAT instruction.\n"
};

// Error definition
jmp_buf errjmp;

// Utility method for throwing errors.
void ahalt(int errid, const char* errdata)
{
	struct errinfo* err = malloc(sizeof(struct errinfo));
	err->errid = errid;
	err->errdata = errdata;
	longjmp(errjmp, (int)err);
}
