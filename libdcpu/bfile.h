/**

	File:		bfile.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU

	Authors:	James Rhodes

	Description:	Provides buffered file access.

**/

#ifndef __DCPU_BFILE_H
#define __DCPU_BFILE_H

#include <stdio.h>
#include "dcpu.h"

typedef struct
{
	FILE* file;
	bool readable;
	bool wrapped;
	int last;
	int eof;
} BFILE;

BFILE* bopen(const char* path, const char* mode);
BFILE* bwrap(FILE* _file, const char* mode);
int bgetc(BFILE* file);
int bputc(int chr, BFILE* file);
long btell(BFILE* file);
int bseek(BFILE* file, long offset, int origin);
int beof(BFILE* file);
void bclose(BFILE* file);

#endif
