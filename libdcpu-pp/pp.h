/**

	File:           pp.h

	Project:        DCPU-16 Tools
	Component:      LibDCPU-pp

	Authors:        James Rhodes

	Description:    Declares the public API for using the preprocessor
	                inline in programs.

**/

#ifndef __DCPU_LIBDCPU_PP_H
#define __DCPU_LIBDCPU_PP_H

#include <bstring.h>

bstring pp_do(freed_bstring path);
void pp_cleanup(freed_bstring path);

#endif
