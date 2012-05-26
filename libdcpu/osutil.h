/**

	File:		osutil.h

	Project:	DCPU-16 Toolchain
	Component:	LibDCPU

	Authors:	James Rhodes

	Description:	Declares functions for interacting with the
			local operating system (such as getting
			directory names, etc.)

**/

#ifndef __DCPU_OSUTIL_H
#define __DCPU_OSUTIL_H

#include <stdio.h>
#include "bstring.h"

bstring osutil_dirname(bstring path);
void osutil_makebinary(FILE* fd);
void osutil_setarg0(freed_bstring arg0);
bstring osutil_getarg0();
bstring osutil_getarg0path();
bstring osutil_getmodulepath();

#endif
