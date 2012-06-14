/**

	File:		bfile.h

	Project:	DCPU-16 Toolchain
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

BFILE* bfopen(const char* path, const char* mode);
BFILE* bfwrap(FILE* _file, const char* mode);
int bfgetc(BFILE* file);
int bfputc(int chr, BFILE* file);
long bftell(BFILE* file);
int bfseek(BFILE* file, long offset, int origin);
int bfeof(BFILE* file);
void bfclose(BFILE* file);
size_t bfread(void* dest, size_t elem, size_t count, BFILE* file);
size_t bfwrite(const void* src, size_t elem, size_t count, BFILE* file);
size_t bfiread(uint16_t* dest, BFILE* file);
size_t bfiwrite(const uint16_t* src, BFILE* file);

#endif
