/**

	File:			ppfind.h

	Project:		DCPU-16 Tools
	Component:		LibDCPU-PP

	Authors:		James Rhodes

	Description:	Declares the functions used to automatically
					locate files based on a set of inclusion paths.

**/

#ifndef __DCPU_LIBDCPU_PP_PPFIND_H
#define __DCPU_LIBDCPU_PP_PPFIND_H

#include <bstrlib.h>

void ppfind_add_path(bstring path);
void ppfind_add_autopath(bstring file);
void ppfind_remove_path(bstring path);
bstring ppfind_locate(bstring path);

#endif