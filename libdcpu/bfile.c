/**

	File:		bfile.c

	Project:	DCPU-16 Tools
	Component:	LibDCPU

	Authors:	James Rhodes

	Description:	Provides buffered file access.

**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "bfile.h"

BFILE* bopen(const char* path, const char* mode)
{
	BFILE* file = (BFILE*)malloc(sizeof(BFILE));
	file->file = fopen(path, mode);
	if (file->file == NULL)
	{
		free(file);
		return NULL;
	}
	file->readable = (mode[0] == 'r' || mode[strlen(mode) - 1] == '+');
	file->last = -1;
	file->eof = false;
	file->wrapped = false;
	if (file->readable)
	{
		file->last = fgetc(file->file);
		file->eof = feof(file->file);
	}
	return file;
}

BFILE* bwrap(FILE* _file, const char* mode)
{
	BFILE* file = (BFILE*)malloc(sizeof(BFILE));
	file->file = _file;
	if (file->file == NULL)
	{
		free(file);
		return NULL;
	}
	file->readable = (mode[0] == 'r' || mode[strlen(mode) - 1] == '+');
	file->last = -1;
	file->eof = false;
	file->wrapped = true;
	if (file->readable)
	{
		file->last = fgetc(file->file);
		file->eof = feof(file->file);
	}
	return file;
}

int bgetc(BFILE* file)
{
	int c;
	assert(file != NULL && file->readable);
	c = file->last;
	file->last = fgetc(file->file);
	file->eof = feof(file->file);
	return c;
}

int bputc(int chr, BFILE* file)
{
	assert(file != NULL);
	return fputc(chr, file->file);
}

long btell(BFILE* file)
{
	assert(file != NULL);
	// TODO: Is this correct?
	if (file->eof)
		return ftell(file->file);
	else
		return ftell(file->file) - 1;
}

int bseek(BFILE* file, long offset, int origin)
{
	assert(file != NULL);
	return fseek(file->file, offset, origin);
}

int beof(BFILE* file)
{
	assert(file != NULL);
	return file->eof;
}

void bclose(BFILE* file)
{
	assert(file != NULL);
	if (!file->wrapped)
		fclose(file->file);
	file->file = NULL;
	file->eof = true;
	file->last = -1;
	file->readable = false;
	free(file);
}