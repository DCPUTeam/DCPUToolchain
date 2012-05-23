/**

	File:		ldbin.c

	Project:	DCPU-16 Toolchain
	Component:	Linker

	Authors:	James Rhodes

	Description:	Provides section bins for storing code in while
			section and output dependencies are resolved.

**/

#include <stdlib.h>
#include "ldbin.h"

///
/// Creates a new linker bin.
///
/// @param name The name of the linker bin.
/// @return The new linker bin.
///
struct ldbin* bin_create(freed_bstring name)
{
	struct ldbin* bin = malloc(sizeof(struct ldbin));
	memset(bin, 0, sizeof(struct ldbin));
	bin->name = bfromcstr("");
	bassign(bin->name, name.ref);
	bautodestroy(name);
	list_init(&bin->words);
	list_attributes_copy(&bin->words, &list_meter_uint16_t, 1);
	list_attributes_comparator(&bin->words, &list_comparator_uint16_t);
	list_attributes_hash_computer(&bin->words, &list_hashcomputer_uint16_t);
	bin->provided = NULL;
	bin->required = NULL;
	bin->adjustment = NULL;
	bin->section = NULL;
	bin->output = NULL;
	return bin;
}

///
/// Destroys a linker bin.
///
/// @param bin The linker bin to destroy.
///
void bin_destroy(struct ldbin* bin)
{
	if (bin->provided != NULL) list_destroy(bin->provided);
	if (bin->required != NULL) list_destroy(bin->required);
	if (bin->adjustment != NULL) list_destroy(bin->adjustment);
	if (bin->section != NULL) list_destroy(bin->section);
	if (bin->output != NULL) list_destroy(bin->output);
	bdestroy(bin->name);
	free(bin);
}

///
/// Writes a word out to the linker bin.
///
/// @param bin The linker bin to store the word in.
///
///
void bin_write(struct ldbin* bin, uint16_t word)
{
	// Append a word to the list (this will copy the value of
	// word after receiving it's reference, so there's no
	// danger of corrupting the list with stack addresses).
	list_append(&bin->words, &word);
}