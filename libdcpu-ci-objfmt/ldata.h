/**

	File:		ldata.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-objfmt

	Authors:	James Rhodes

	Description:	Declares structures and functions used to
			retrieve the linker table from an object file.

**/

#ifndef __DCPU_LNK_LDATA_H
#define __DCPU_LNK_LDATA_H

#include <stdint.h>
#include <stdio.h>

#define LABEL_END 0
#define LABEL_PROVIDED 1
#define LABEL_REQUIRED 2
#define LABEL_ADJUSTMENT 3

struct ldata_entry
{
	char label[256];
	uint8_t mode;
	uint16_t address;
};

static const char ldata_objfmt[] = "OBJECT-FORMAT-1.0";
struct ldata_entry* ldata_read(FILE* in);
void ldata_write(FILE* out, struct ldata_entry* entry);

#endif
