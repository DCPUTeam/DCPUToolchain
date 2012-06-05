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
#include <dcpu.h>

struct ldbin
{
	bstring name;
	list_t* provided;
	list_t* required;
	list_t* adjustment;
	list_t* section;
	list_t* output;
	list_t* symbols;
	list_t words;
};

struct ldbin* bin_create(freed_bstring name);
void bin_destroy(struct ldbin* bin);
void bin_write(struct ldbin* bin, uint16_t word);

void bin_move(list_t* all, struct ldbin* to, struct ldbin* from, size_t at, size_t offset, size_t count);
void bin_replace(list_t* all, struct ldbin* to, struct ldbin* from, size_t at, size_t erase, size_t offset, size_t count);
void bin_append(list_t* all, struct ldbin* to, struct ldbin* from, size_t offset, size_t count);
void bin_insert(list_t* all, struct ldbin* to, struct ldbin* from, size_t at, size_t offset, size_t count);
void bin_remove(list_t* all, struct ldbin* bin, size_t offset, size_t count);

void bin_info_insert(list_t* all, struct ldbin* to, list_t* tolist, struct ldbin* from, list_t* fromlist, bool isAdjustment, bool isOutput, size_t at, size_t offset, size_t count);
void bin_info_remove(list_t* all, struct ldbin* bin, list_t* list, bool isAdjustment, size_t offset, size_t count);
void bin_info_insert_symbols(struct ldbin* to, struct ldbin* from, size_t at, size_t offset, size_t count);
void bin_info_remove_symbols(struct ldbin* bin, size_t offset, size_t count);

#endif
