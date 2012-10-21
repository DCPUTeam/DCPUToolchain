/**

	File:		ppfind.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-pp

	Authors:	James Rhodes

	Description:	Declares the functions used to automatically
			locate files based on a set of inclusion paths.

**/

#ifndef __DCPU_LIBDCPU_PP_PPFIND_H
#define __DCPU_LIBDCPU_PP_PPFIND_H

#include <bstring.h>

void ppfind_add_path(freed_bstring path);
void ppfind_add_autopath(freed_bstring file);
void ppfind_remove_path(freed_bstring path);
bstring ppfind_locate(freed_bstring path);

#endif
