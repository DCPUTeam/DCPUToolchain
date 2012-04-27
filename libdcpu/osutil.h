/**

	File:			osutil.h

	Project:		DCPU-16 Tools
	Component:		LibDCPU

	Authors:		James Rhodes

	Description:	Declares functions for interacting with the
	                local operating system (such as getting
					directory names, etc.)

**/

#ifndef __DCPU_OSUTIL_H
#define __DCPU_OSUTIL_H

#include <stdio.h>
#include <bstrlib.h>

bstring osutil_dirname(bstring path);
void osutil_makebinary(FILE* fd);

#endif