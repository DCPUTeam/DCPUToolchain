/**

	File:		objfile.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-objfmt

	Authors:	James Rhodes

	Description:	Declares functions for parsing, reading and writing
			to and from object files.

**/

#ifndef __DCPU_LNK_OBJFILE_H
#define __DCPU_LNK_OBJFILE_H

#include <stdint.h>
#include "lprov.h"

void objfile_load(const char* filename, FILE* in, uint16_t* offset, struct lprov_entry** provided, struct lprov_entry** required, struct lprov_entry** adjustment);
void objfile_save(FILE* out, struct lprov_entry* provided, struct lprov_entry* required, struct lprov_entry* adjustment);

#endif
