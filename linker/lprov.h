/**

	File:			lprov.h

	Project:		DCPU-16 Tools
	Component:		Linker

	Authors:		James Rhodes

	Description:	Declares structures and functions for the
					provided labels linked-list.

**/

#ifndef __DCPU_LNK_LPROV_H
#define __DCPU_LNK_LPROV_H

#include <stdint.h>

struct lprov_entry
{
	char* label;
	uint16_t address;
	struct lprov_entry* next;
};

struct lprov_entry* lprov_create(char* label, uint16_t address);

#endif