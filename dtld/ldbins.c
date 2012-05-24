/**

	File:		ldbins.c

	Project:	DCPU-16 Toolchain
	Component:	Linker

	Authors:	James Rhodes

	Description:	Provides section bins for storing code in while
			section and output dependencies are resolved.

**/

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <simclist.h>
#include <ldata.h>
#include <objfile.h>
#include <assert.h>
#include <debug.h>
#include "ldbins.h"
#include "ldbin.h"
#include "ldconv.h"

///
/// The global bin storage.
///
struct
{
	list_t bins;
} ldbins;

///
/// Finds a bin in the bin list by it's name.
///
int bin_seeker(const void* el, const void* indicator)
{
	if (el == NULL || indicator == NULL) return 0;
	if (biseq(((struct ldbin*)el)->name, (bstring)indicator))
		return 1;
	else
		return 0;
}

///
/// Initializes the linker bin system.
///
void bins_init()
{
	list_init(&ldbins.bins);
	list_attributes_seeker(&ldbins.bins, &bin_seeker);
}

///
/// Adds a new bin to the system with the specified name.
///
/// Adds a new bin to the system with the specified name and link
/// information.
///
/// @param name The bin name.
/// @param provided A linked list of provided labels.
/// @param required A linked list of required labels.
/// @param adjustment A linked list of addresses that need adjusting.
/// @param section A linked list of defined sections.
/// @param output A linked list of defined output areas.
///
struct ldbin* bins_add(freed_bstring name, struct lprov_entry* provided, struct lprov_entry* required, struct lprov_entry* adjustment, struct lprov_entry* section, struct lprov_entry* output)
{
	struct ldbin* bin = bin_create(name, false);
	bin->provided = list_convert(provided);
	bin->required = list_convert(required);
	bin->adjustment = list_convert(adjustment);
	bin->section = list_convert(section);
	bin->output = list_convert(output);
	list_append(&ldbins.bins, bin);
	return bin;
}

///
/// Loads a new bin by reading a linker object from file.
///
/// Adds a new bin by reading in a linker object stored on disk and
/// automatically handling loading bytes and linker information into
/// the bin.
///
/// @param path The path to load.
/// @return Whether the bin was loaded successfully.
///
bool bins_load(freed_bstring path)
{
	uint16_t offset = 0, store;
	struct lprov_entry* required = NULL;
	struct lprov_entry* provided = NULL;
	struct lprov_entry* adjustment = NULL;
	struct lprov_entry* section = NULL;
	struct lprov_entry* output = NULL;
	struct ldbin* bin;
	FILE* in;
	char* test;

	// Open the input file.
	in = fopen(path.ref->data, "rb");

	if (in == NULL)
	{
		// Handle the error.
		return false;
	}

	// Is this the object format?
	test = malloc(strlen(ldata_objfmt) + 1);
	memset(test, 0, strlen(ldata_objfmt) + 1);
	fread(test, 1, strlen(ldata_objfmt), in);
	fseek(in, 1, SEEK_CUR);

	if (strcmp(test, ldata_objfmt) != 0)
	{
		// Handle the error.
		return false;
	}

	free(test);

	// Load only the provided label entries into memory.
	objfile_load(path.ref->data, in, &offset, &provided, &required, &adjustment, &section, &output);

	// Add the new bin.
	bin = bins_add(path, provided, required, adjustment, section, output);

	// Copy all of the input file's data into the output
	// file, word by word.
	while (true)
	{
		// Read a word into the bin.  The reason that
		// we break inside the loop is that we are reading
		// two bytes at a time and thus the EOF flag doesn't
		// get set until we actually attempt to read past
		// the end-of-file.  If we don't do this, we get a
		// double read of the same data.
		fread(&store, sizeof(uint16_t), 1, in);
		if (feof(in))
			break;
		bin_write(bin, store);
	}

	// Close the file.
	fclose(in);

	// TODO: Free the list data in the struct lprov_entry
	// pointers (since it's cloned by the bin system).

	return true;
}

///
/// Saves the bin with the specified name to the specified path.
///
/// @param name The name of the bin to save.
/// @param path The path to save the bin to.
///
void bins_save(freed_bstring name, freed_bstring path)
{
	FILE* out;
	struct ldbin* bin = list_seek(&ldbins.bins, name.ref);
	assert(bin != NULL);

	// Open the output file.
	out = fopen(path.ref->data, "wb");

	// Write each byte from the bin.
	list_iterator_start(&bin->words);
	while (list_iterator_hasnext(&bin->words))
		fwrite(list_iterator_next(&bin->words), sizeof(uint16_t), 1, out);
	list_iterator_stop(&bin->words);

	// Close the output file.
	fclose(out);

	// Free strings.
	bautodestroy(name);
	bautodestroy(path);
}

///
/// Writes a word of data into the specified bin.
///
/// @param path The path to load.
/// @return Whether the bin was loaded successfully.
///
bool bins_write(freed_bstring name, uint16_t word)
{
	struct ldbin* bin = list_seek(&ldbins.bins, name.ref);
	if (bin == NULL)
	{
		bautodestroy(name);
		return false;
	}
	bin_write(bin, word);
	return true;
}

///
/// Splits the currently loaded bins into sectioned bins.
///
void bins_sectionize()
{
	struct lconv_entry* entry;
	struct ldbin* bin;
	struct ldbin* target;
	list_t create;
	size_t i;
	int steal, stolen, index, base;
	bstring name;

	list_init(&create);
	list_attributes_seeker(&create, &bin_seeker);

	// Print result information.
	for (i = 0; i < list_size(&ldbins.bins); i++)
	{
		bin = list_get_at(&ldbins.bins, i);
		printd(LEVEL_VERBOSE, "start bin: %s\n", bin->name->data);
		bin->provided != NULL ? printd(LEVEL_VERBOSE, "	 total provided: %u\n", list_size(bin->provided)) : false;
		bin->required != NULL ? printd(LEVEL_VERBOSE, "	 total required: %u\n", list_size(bin->required)) : false;
		bin->adjustment != NULL ? printd(LEVEL_VERBOSE, "  total adjustment: %u\n", list_size(bin->adjustment)) : false;
		bin->section != NULL ? printd(LEVEL_VERBOSE, "	total sections: %u\n", list_size(bin->section)) : false;
		bin->output != NULL ? printd(LEVEL_VERBOSE, "  total outputs: %u\n", list_size(bin->output)) : false;
		printd(LEVEL_VERBOSE, "	 total words: 0x%04X\n", list_size(&bin->words));
		list_iterator_start(&bin->words);
		while (list_iterator_hasnext(&bin->words))
			printd(LEVEL_VERBOSE, "	   0x%04X\n", *(uint16_t*)list_iterator_next(&bin->words));
		list_iterator_stop(&bin->words);
		printd(LEVEL_VERBOSE, "	 \n");
	}

	// Copy words into appropriate bins.
	list_iterator_start(&ldbins.bins);
	while (list_iterator_hasnext(&ldbins.bins))
	{
		bin = list_iterator_next(&ldbins.bins);

		// Search all of the sections in this bin.
		assert(bin->section != NULL);
		list_iterator_start(bin->section);
		while (list_iterator_hasnext(bin->section))
		{
			entry = list_iterator_next(bin->section);

			// Create target section bin if it doesn't
			// already exist.
			name = bfromcstr("SECTION ");
			bconcat(name, entry->label);
			if (list_seek(&create, name) == NULL)
			{
				target = bin_create(bautofree(name), false);
				target->provided = list_create();
				target->required = list_create();
				target->adjustment = list_create();
				target->output = list_create();
				list_append(&create, target);
			}
			else
				bdestroy(name);
		}
		list_iterator_stop(bin->section);
	}
	list_iterator_stop(&ldbins.bins);

	// For each of the file bins, move the code that they
	// have in sections into the section bins.
	list_iterator_start(&ldbins.bins);
	while (list_iterator_hasnext(&ldbins.bins))
	{
		bin = list_iterator_next(&ldbins.bins);

		// Search all of the sections in this bin.
		stolen = 0;
		assert(bin->section != NULL);
		list_sort(bin->section, 1);
		for (i = 0; i < list_size(bin->section); i++)
		{
			// Work out the target bin.
			name = bfromcstr("SECTION ");
			bconcat(name, ((struct lconv_entry*)list_get_at(bin->section, i))->label);
			target = list_seek(&create, name);
			assert(target != NULL);
			bdestroy(name);

			// Calculate how many bytes to steal from this section.
			if (i == list_size(bin->section) - 1)
			{
				// Steal until end-of-bin.
				steal = list_size(&bin->words) -
					(((struct lconv_entry*)list_get_at(bin->section, i))->address - stolen);
			}
			else
			{
				// Steal up to the next section.
				steal = (((struct lconv_entry*)list_get_at(bin->section, i + 1))->address - stolen) -
					(((struct lconv_entry*)list_get_at(bin->section, i))->address - stolen);
			}

			// Get the index from which to extract.
			index = ((struct lconv_entry*)list_get_at(bin->section, i))->address - stolen;
			base = list_size(&target->words);

			bin_move(target, bin, base, index, steal);
			stolen += steal;
		}
	}
	list_iterator_stop(&ldbins.bins);

	// Append new bins to the bin list and free
	// memory of old list.
	list_iterator_start(&create);
	while (list_iterator_hasnext(&create))
		list_append(&ldbins.bins, list_iterator_next(&create));
	list_iterator_stop(&create);
	list_destroy(&create);

	// Print result information.
	for (i = 0; i < list_size(&ldbins.bins); i++)
	{
		bin = list_get_at(&ldbins.bins, i);
		printd(LEVEL_VERBOSE, "end bin: %s\n", bin->name->data);
		bin->provided != NULL ? printd(LEVEL_VERBOSE, "	 total provided: %u\n", list_size(bin->provided)) : false;
		bin->required != NULL ? printd(LEVEL_VERBOSE, "	 total required: %u\n", list_size(bin->required)) : false;
		bin->adjustment != NULL ? printd(LEVEL_VERBOSE, "  total adjustment: %u\n", list_size(bin->adjustment)) : false;
		bin->section != NULL ? printd(LEVEL_VERBOSE, "	total sections: %u\n", list_size(bin->section)) : false;
		bin->output != NULL ? printd(LEVEL_VERBOSE, "  total outputs: %u\n", list_size(bin->output)) : false;
		printd(LEVEL_VERBOSE, "	 total words: 0x%04X\n", list_size(&bin->words));
		list_iterator_start(&bin->words);
		while (list_iterator_hasnext(&bin->words))
			printd(LEVEL_VERBOSE, "	   0x%04X\n", *(uint16_t*)list_iterator_next(&bin->words));
		list_iterator_stop(&bin->words);
		printd(LEVEL_VERBOSE, "	 \n");
	}
}

///
/// Flattens all of the current bins into a single contigious
/// bin.
///
void bins_flatten(freed_bstring name)
{
	struct lconv_entry* entry;
	struct ldbin* target;
	struct ldbin* bin;
	bstring start, desired;
	size_t i;

	// Create the output bin.
	target = bin_create(name, false);
	target->provided = list_create();
	target->required = list_create();
	target->adjustment = list_create();
	target->output = list_create();

	// Loop through all of the current bins and evaluate them.
	list_iterator_start(&ldbins.bins);
	while (list_iterator_hasnext(&ldbins.bins))
	{
		bin = list_iterator_next(&ldbins.bins);

		list_iterator_start(bin->output);
		while (list_iterator_hasnext(bin->output))
		{
			entry = list_iterator_next(bin->output);

			printd(LEVEL_DEBUG, "%s: will output %s at 0x%4X\n", bin->name->data, entry->label->data, entry->address);
		}
		list_iterator_stop(bin->output);

		// Skip if the name begins with SECTION.
		start = bmidstr(bin->name, 0, 8);
		if (biseqcstr(start, "SECTION "))
		{
			bdestroy(start);
			continue;
		}
		bdestroy(start);

		// Move all of the code from this bin into the
		// created bin.
		bin_move(target, bin, list_size(&target->words), 0, list_size(&bin->words));

	}
	list_iterator_stop(&ldbins.bins);

	// Sort the output bins in *reverse* order since we want
	// to insert the last output first.
	list_sort(target->output, -1);

	// Search for all of the output entries in the flattened
	// output bin.
	list_iterator_start(target->output);
	while (list_iterator_hasnext(target->output))
	{
		entry = list_iterator_next(target->output);

		// Find the section that matches.
		desired = bfromcstr("SECTION ");
		bconcat(desired, entry->label);
		bin = list_seek(&ldbins.bins, desired);

		// TODO: Throw a proper error.
		assert(bin != NULL);

		// Insert the required code.
		bin_insert(target, bin, entry->address, 0, list_size(&bin->words));
	}
	list_iterator_stop(target->output);

	// Delete all of the bins.
	// TODO: Free data stored in the list before clearing.
	//for (i = list_size(&ldbins.bins) - 1; i >= 0; i--)
	//	bin_destroy(list_get_at(&ldbins.bins, i));
	list_clear(&ldbins.bins);

	// Add the flattened bin to the list of bins.
	list_append(&ldbins.bins, target);

	// Print result information.
	for (i = 0; i < list_size(&ldbins.bins); i++)
	{
		bin = list_get_at(&ldbins.bins, i);
		printd(LEVEL_VERBOSE, "flattened bin: %s\n", bin->name->data);
		bin->provided != NULL ? printd(LEVEL_VERBOSE, "	 total provided: %u\n", list_size(bin->provided)) : false;
		bin->required != NULL ? printd(LEVEL_VERBOSE, "	 total required: %u\n", list_size(bin->required)) : false;
		bin->adjustment != NULL ? printd(LEVEL_VERBOSE, "  total adjustment: %u\n", list_size(bin->adjustment)) : false;
		bin->section != NULL ? printd(LEVEL_VERBOSE, "	total sections: %u\n", list_size(bin->section)) : false;
		bin->output != NULL ? printd(LEVEL_VERBOSE, "  total outputs: %u\n", list_size(bin->output)) : false;
		printd(LEVEL_VERBOSE, "	 total words: 0x%04X\n", list_size(&bin->words));
		list_iterator_start(&bin->words);
		while (list_iterator_hasnext(&bin->words))
			printd(LEVEL_VERBOSE, "	   0x%04X\n", *(uint16_t*)list_iterator_next(&bin->words));
		list_iterator_stop(&bin->words);
		printd(LEVEL_VERBOSE, "	 \n");
	}
}

///
/// Resolves all of the required and provided labels in a program.  The bins
/// must be flattened at this point.
///
/// @param Whether the provided label entries should be kept in the flattened
///	   bin for re-exporting (for example in static libraries).
///
void bins_resolve(bool keepProvided)
{
	struct lconv_entry* required;
	struct lconv_entry* provided;
	struct ldbin* bin;
	uint16_t* word;
	size_t i;

	// Get the first bin.
	assert(list_size(&ldbins.bins) == 1);
	bin = list_get_at(&ldbins.bins, 0);

	// Iterator over all required values.
	list_iterator_start(bin->required);
	while (list_iterator_hasnext(bin->required))
	{
		// Get the required / provided matching labels.
		required = list_iterator_next(bin->required);
		provided = list_seek(bin->provided, required->label);

		// TODO: Throw a proper error.
		assert(required != NULL);
		if (provided == NULL)
		{
			printd(LEVEL_ERROR, "could not find label %s.\n", required->label->data);
			exit(1);
		}

		// Insert the required code.
		word = list_get_at(&bin->words, required->address);
		*word = provided->address;

		printd(LEVEL_DEBUG, "resolve: %s (0x%4X) -> 0x%4X\n", required->label->data, required->address, provided->address);
	}
	list_iterator_stop(bin->required);

	// Delete all of the required entries.
	while (list_size(bin->required) > 0)
	{
		required = list_extract_at(bin->required, 0);
		bdestroy(required->label);
		free(required);
	}

	if (!keepProvided)
	{
		// Delete all of the provided entries.
		while (list_size(bin->provided) > 0)
		{
			provided = list_extract_at(bin->provided, 0);
			bdestroy(provided->label);
			free(provided);
		}
	}

	// Print result information.
	for (i = 0; i < list_size(&ldbins.bins); i++)
	{
		bin = list_get_at(&ldbins.bins, i);
		printd(LEVEL_VERBOSE, "resolved bin: %s\n", bin->name->data);
		bin->provided != NULL ? printd(LEVEL_VERBOSE, "	 total provided: %u\n", list_size(bin->provided)) : false;
		bin->required != NULL ? printd(LEVEL_VERBOSE, "	 total required: %u\n", list_size(bin->required)) : false;
		bin->adjustment != NULL ? printd(LEVEL_VERBOSE, "  total adjustment: %u\n", list_size(bin->adjustment)) : false;
		bin->section != NULL ? printd(LEVEL_VERBOSE, "	total sections: %u\n", list_size(bin->section)) : false;
		bin->output != NULL ? printd(LEVEL_VERBOSE, "  total outputs: %u\n", list_size(bin->output)) : false;
		printd(LEVEL_VERBOSE, "	 total words: 0x%04X\n", list_size(&bin->words));
		list_iterator_start(&bin->words);
		while (list_iterator_hasnext(&bin->words))
			printd(LEVEL_VERBOSE, "	   0x%04X\n", *(uint16_t*)list_iterator_next(&bin->words));
		list_iterator_stop(&bin->words);
		printd(LEVEL_VERBOSE, "	 \n");
	}
}