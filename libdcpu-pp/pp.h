/**

	File:			pp.h

	Project:		DCPU-16 Tools
	Component:		LibDCPU-PP

	Authors:		James Rhodes

	Description:	Declares the public API for using the preprocessor
					inline in programs.

**/

#ifndef __DCPU_LIBDCPU_PP_H
#define __DCPU_LIBDCPU_PP_H

#include <bstrlib.h>

bstring pp_do(bstring path);
void pp_cleanup(bstring path);

#endif