/**

	File:		micro.h

	Project:	DCPU-16 Tools
	Component:	Image Smasher

	Authors:	James Rhodes

	Description:	Defines function for getting a filesystem writer
			for the micro filesystem format.

**/

#ifndef __DCPU_IMG_MICRO_H
#define __DCPU_IMG_MICRO_H

#include "base.h"

struct fs_writer* writer_get_micro();

#endif