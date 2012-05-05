/**

	File:		version.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU

	Authors:	James Rhodes

	Description:	Declares the functions that provide version
			information.

**/

#ifndef __DCPU_VERSION_H
#define __DCPU_VERSION_H

#include "bstring.h"

bstring version_get();
void version_print(freed_bstring application);

#endif
