/**

	File:		ldbin.h

	Project:	DCPU-16 Toolchain
	Component:	Linker

	Authors:	James Rhodes

	Description:	Provides section bins for storing code in while
			section and output dependencies are resolved.

**/

#ifndef __DCPU_LD_BIN_H
#define __DCPU_LD_BIN_H

#include <stdint.h>
#include <bstring.h>
#include <simclist.h>
#include <lprov.h>

struct ldbin
{
	bstring name;
	list_t* provided;
	list_t* required;
	list_t* adjustment;
	list_t* section;
	list_t* output;
	list_t words;
};

struct ldbin* bin_create(freed_bstring name);
void bin_destroy(struct ldbin* bin);
void bin_write(struct ldbin* bin, uint16_t word);

#endif