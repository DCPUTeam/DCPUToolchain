/**

	File:			osutil.c

	Project:		DCPU-16 Tools
	Component:		LibDCPU

	Authors:		James Rhodes

	Description:	Defines functions for interacting with the
	                local operating system (such as getting
					directory names, etc.)

**/

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#else
#include <libgen.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <bstrlib.h>
#include "osutil.h"

#ifdef _WIN32
char* dirname(char* path)
{
	// FIXME: This assumes the resulting path will always
	// be shorter than the original (which it should be
	// given that we're only returning a component of it, right?)
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	_splitpath(path, drive, dir, NULL, NULL);
	strcpy(path, drive);
	strcpy(path + strlen(path), dir);
	return path;
}
#endif

bstring osutil_dirname(bstring path)
{
	bstring bpath;
	char* cpath;
		
	cpath = bstr2cstr(path, '0');
	dirname(cpath);
	bpath = bfromcstr(cpath);
	bcstrfree(cpath);

	return bpath;
}

void osutil_makebinary(FILE* fd)
{
#ifdef _WIN32
		_setmode( _fileno( stdout ), _O_BINARY );
#endif
}