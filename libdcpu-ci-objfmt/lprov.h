/**

	File:           lprov.h

	Project:        DCPU-16 Tools
	Component:      LibDCPU-ci-objfmt

	Authors:        James Rhodes

	Description:    Declares structures and functions for the
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
struct lprov_entry* lprov_find_by_label(struct lprov_entry* first, char* label);
struct lprov_entry* lprov_find_by_address(struct lprov_entry* first, uint16_t address);

#endif
