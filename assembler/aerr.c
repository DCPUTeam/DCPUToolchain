//
// DCPU Assembler by James Rhodes
//
// Error definitions
//

#include <setjmp.h>
#include "aerr.h"

// Error strings
const char* err_strings[6] = 
{
	"assembler: generic assembling error.\n",
	"assembler: label '%s' not found.\n",
	"assembler: attempted to output NULL operation.\n",
	"assembler: can not use register '%s' in brackets.\n",
	"assembler: can not use register '%s' as next value.\n",
	"assembler: relocation table too large.\n"
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
