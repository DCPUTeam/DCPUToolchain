//
// DCPU Assembler by James Rhodes
//
// Error definitions
//

#ifndef __DCPU_ASM_AERR_H
#define __DCPU_ASM_AERR_H

#include <setjmp.h>

// Error codes
#define ERR_GENERIC 0
#define ERR_LABEL_NOT_FOUND 1
#define ERR_OUTPUT_NULL 2
#define ERR_BRACKETED_REGISTER_UNSUPPORTED 3
#define ERR_NEXTED_REGISTER_UNSUPPORTED 4
#define ERR_RELOCATION_TABLE_TOO_LARGE 5
#define ERR_DAT_UNSUPPORTED_PARAMETER 6
#define ERR_GEN_UNSUPPORTED_PARAMETER 7

// Variables
extern const char* err_strings[];
extern jmp_buf errjmp;

// Structure for holding error information
struct errinfo
{
	int errid;
	const char* errdata;
};

// Utility method for throwing errors.
void ahalt(int errid, const char* errdata);

#endif