/**

	File:		ldbins.h

	Project:	DCPU-16 Toolchain
	Component:	Linker

	Authors:	James Rhodes

	Description:	Provides a section bin list upon which operations
			can then be performed.

**/

#ifndef __DCPU_LD_BINS_H
#define __DCPU_LD_BINS_H

#include <stdint.h>
#include <bstring.h>
#include <lprov.h>
#include <dcpu.h>
#include "ldbin.h"

void bins_init();
struct ldbin* bins_add(freed_bstring name, struct lprov_entry* provided, struct lprov_entry* required, struct lprov_entry* adjustment, struct lprov_entry* section, struct lprov_entry* output);
bool bins_load(freed_bstring path);
void bins_save(freed_bstring name, freed_bstring path);
bool bins_write(freed_bstring name, uint16_t word);
void bins_sectionize();
void bins_flatten(freed_bstring name);
void bins_resolve(bool keepProvided);

#endif