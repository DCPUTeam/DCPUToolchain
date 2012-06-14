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
#include <bfile.h>

struct fs_writer
{
	void (*init)(BFILE* out);
	void (*write_kernel)(BFILE* out, BFILE* in, int count);
	void (*write_file)(BFILE* out, BFILE* in, freed_bstring name);
	void (*close)(BFILE* out);
};

#endif