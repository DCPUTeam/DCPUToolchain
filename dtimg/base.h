/**

	File:		base.h

	Project:	DCPU-16 Tools
	Component:	Image Smasher

	Authors:	James Rhodes

	Description:	Defines universal structure for calling operations
			on filesystem writers.

**/

#ifndef __DCPU_IMG_BASE_H
#define __DCPU_IMG_BASE_H

#include <stdio.h>
#include <bstring.h>

struct fs_writer
{
	void (*init)(FILE* out);
	void (*write_kernel)(FILE* out, FILE* in, int count);
	void (*write_file)(FILE* out, FILE* in, freed_bstring name);
	void (*close)(FILE* out);
};

#endif