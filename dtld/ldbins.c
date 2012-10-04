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
#include <iio.h>
#include <ldata.h>
#include <objfile.h>
#include <assert.h>
#include <debug.h>
#include "ldbins.h"
#include "ldbin.h"
#include "ldlua.h"
#include "ldconv.h"
#include "ddata.h"

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
	struct ldbin* bin = bin_create(name);
	bin->provided = list_convert(provided);
	bin->required = list_convert(required);
	bin->adjustment = list_convert(adjustment);
	bin->section = list_convert(section);
	bin->output = list_convert(output);
	bin->symbols = dbgfmt_create_list();
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
bool bins_load(freed_bstring path, bool loadDebugSymbols, const char* debugSymbolExtension)
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
	bstring sympath;
	int sympathi, sympathj, sympathk;

	// Open the input file.
	in = fopen(path.ref->data, "rb");

	if (in == NULL)
	{
		// Handle the error.
		printd(LEVEL_ERROR, "error: unable to open '%s'.\n", path.ref->data);
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

	// Determine the symbol path before we hand the main
	// input path over to bins_add.
	if (loadDebugSymbols)
	{
		sympath = bfromcstr("");
		bconcat(sympath, path.ref);
		sympathi = bstrrchr(sympath, '.');
		sympathj = bstrrchr(sympath, '/');
		sympathk = bstrrchr(sympath, '\\');
		if (sympathi != BSTR_ERR && (sympathj == BSTR_ERR || sympathi > sympathj) && (sympathk == BSTR_ERR || sympathi > sympathk))
			btrunc(sympath, sympathi);
		bconchar(sympath, '.');
		bcatcstr(sympath, debugSymbolExtension);
	}

	// Load only the provided label entries into memory.
	objfile_load(path.ref->data, in, &offset, &provided, &required, &adjustment, &section, &output);

	// Add the new bin.
	bin = bins_add(path, provided, required, adjustment, section, output);

	// Load all of the debugging symbols if requested.
	if (loadDebugSymbols)
	{
		dbgfmt_free(bin->symbols); // bins_add will have allocated a list that we need to free
		bin->symbols = dbgfmt_read(sympath, false);
		bdestroy(sympath);
	}

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
		iread(&store, in);
		if (feof(in))
			break;
		bin_write(bin, store);
	}

	// Close the file.
	fclose(in);

	// Free the list data in the struct lprov_entry
	// pointers (since it's cloned by the bin system).  Don't
	// free the debugging symbols however, as they are not
	// cloned.
	lprov_free(provided);
	lprov_free(required);
	lprov_free(adjustment);
	lprov_free(section);
	lprov_free(output);

	return true;
}

///
/// Saves the bin with the specified name to the specified path.
///
/// @param name The name of the bin to save.
/// @param path The path to save the bin to.
///
void bins_save(freed_bstring name, freed_bstring path, freed_bstring target, bool keepOutputs, const char* symbolFilename)
{
	FILE* out;
	struct ldbin* bin = list_seek(&ldbins.bins, name.ref);
	struct lprov_entry* provided = NULL;
	struct lprov_entry* required = NULL;
	struct lprov_entry* adjustment = NULL;
	struct lprov_entry* section = NULL;
	struct lprov_entry* outputs = NULL;
	bstring sympath;
	assert(bin != NULL);

	// Open the output file.
	out = fopen(path.ref->data, "wb");
	if (out == NULL)
	{
		bautodestroy(target);
		bautodestroy(name);
		bautodestroy(path);
		printd(LEVEL_ERROR, "error: unable to open output path to save bins.");
		exit(1);
	}

	// Check to see whether we need to write out
	// the linker table.
	if (biseqcstr(target.ref, "static"))
	{
		// FIXME: Free the result of list_revert (or better yet
		// convert objfmt library to use simclist).
		fwrite(ldata_objfmt, 1, strlen(ldata_objfmt) + 1, out);
		provided = list_revert(bin->provided);
		required = list_revert(bin->required);
		adjustment = list_revert(bin->adjustment);
		section = list_revert(bin->section);
		if (keepOutputs)
			outputs = list_revert(bin->output);
		objfile_save(out, provided, required, adjustment, section, outputs);
		lprov_free(provided);
		lprov_free(required);
		lprov_free(adjustment);
		lprov_free(section);
		lprov_free(outputs);
	}

	// Write each byte from the bin.
	list_iterator_start(&bin->words);
	while (list_iterator_hasnext(&bin->words))
		iwrite(list_iterator_next(&bin->words), out);
	list_iterator_stop(&bin->words);

	// Close the output file.
	fclose(out);

	// If the symbol filename was provided, write the debugging symbols
	// to that as well.
	if (symbolFilename != NULL)
	{
		sympath = bfromcstr(symbolFilename);
		dbgfmt_write(sympath, bin->symbols);
		bdestroy(sympath);
	}

	// Free strings.
	bautodestroy(target);
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
/// Updates adjustment targets to associate their bin target information.
///
void bins_associate()
{
	struct ldbin* bin;
	struct lconv_entry* entry;

	// Apply to all bins.
	list_iterator_start(&ldbins.bins);
	while (list_iterator_hasnext(&ldbins.bins))
	{
		bin = list_iterator_next(&ldbins.bins);
		if (bin->adjustment == NULL) continue;

		// Search all of the sections in this bin.
		list_iterator_start(bin->adjustment);
		while (list_iterator_hasnext(bin->adjustment))
		{
			entry = list_iterator_next(bin->adjustment);

			// Associate the entry's bin name with the name
			// of the current bin.
			entry->bin = bin->name; // TODO: Should we copy the bstring?
		}
		list_iterator_stop(bin->adjustment);
	}
	list_iterator_stop(&ldbins.bins);
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
	size_t i, dk;
	int steal, stolen, index, base;
	bstring name;

	list_init(&create);
	list_attributes_seeker(&create, &bin_seeker);

	// Print result information.
	for (i = 0; i < list_size(&ldbins.bins); i++)
	{
		bin = list_get_at(&ldbins.bins, i);
		printd(LEVEL_VERBOSE, "start bin: %s\n", bin->name->data);
		bin->provided != NULL ? printd(LEVEL_VERBOSE,	"    total provided: %u\n", list_size(bin->provided)) : false;
		bin->required != NULL ? printd(LEVEL_VERBOSE,	"    total required: %u\n", list_size(bin->required)) : false;
		bin->adjustment != NULL ? printd(LEVEL_VERBOSE, "    total adjustment: %u\n", list_size(bin->adjustment)) : false;
		bin->section != NULL ? printd(LEVEL_VERBOSE,	"    total sections: %u\n", list_size(bin->section)) : false;
		bin->output != NULL ? printd(LEVEL_VERBOSE,	"    total outputs: %u\n", list_size(bin->output)) : false;
		bin->symbols != NULL ? printd(LEVEL_VERBOSE,	"    total symbols: %u\n", list_size(bin->symbols)) : false;
		printd(LEVEL_VERBOSE,				"    total words: 0x%04X\n", list_size(&bin->words));
		list_iterator_start(&bin->words);
		dk = 0;
		while (list_iterator_hasnext(&bin->words))
			printd(LEVEL_EVERYTHING,		"	 0x%04X: 0x%04X\n", dk++, *(uint16_t*)list_iterator_next(&bin->words));
		list_iterator_stop(&bin->words);
		printd(LEVEL_EVERYTHING,			"	 ------: ------\n");
		if (bin->adjustment != NULL)
		{
			list_iterator_start(bin->adjustment);
			while (list_iterator_hasnext(bin->adjustment))
			{
				entry = list_iterator_next(bin->adjustment);
				assert(list_get_at(&bin->words, entry->address) != NULL);
				printd(LEVEL_EVERYTHING,	"	 0x%04X: 0x%04X (%s)\n", entry->address, *(uint16_t*)list_get_at(&bin->words, entry->address), entry->bin->data);
			}
			list_iterator_stop(bin->adjustment);
		}
		printd(LEVEL_EVERYTHING,			"	 ------: ------\n");
		if (bin->symbols != NULL)
		{
			list_iterator_start(bin->symbols);
			while (list_iterator_hasnext(bin->symbols))
				printd(LEVEL_EVERYTHING,	"	 0x%04X: DEBUG SYMBOL\n", dbgfmt_get_symbol_address(list_iterator_next(bin->symbols)));
			list_iterator_stop(bin->symbols);
		}
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
				target = bin_create(bautofree(name));
				target->provided = list_create();
				target->required = list_create();
				target->adjustment = list_create();
				target->output = list_create();
				target->symbols = dbgfmt_create_list();
				list_append(&create, target);
				printd(LEVEL_DEBUG, "created section %s\n", target->name->data);
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

			printd(LEVEL_DEBUG, "taking 0x%04X words to %s from 0x%04X to 0x%04X in %s\n", steal, target->name->data, index, base, bin->name->data);
			bin_move(&ldbins.bins, target, bin, base, index, steal);
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
		bin->provided != NULL ? printd(LEVEL_VERBOSE,	"    total provided: %u\n", list_size(bin->provided)) : false;
		bin->required != NULL ? printd(LEVEL_VERBOSE,	"    total required: %u\n", list_size(bin->required)) : false;
		bin->adjustment != NULL ? printd(LEVEL_VERBOSE, "    total adjustment: %u\n", list_size(bin->adjustment)) : false;
		bin->section != NULL ? printd(LEVEL_VERBOSE,	"    total sections: %u\n", list_size(bin->section)) : false;
		bin->output != NULL ? printd(LEVEL_VERBOSE,	"    total outputs: %u\n", list_size(bin->output)) : false;
		bin->symbols != NULL ? printd(LEVEL_VERBOSE,	"    total symbols: %u\n", list_size(bin->symbols)) : false;
		printd(LEVEL_VERBOSE,				"    total words: 0x%04X\n", list_size(&bin->words));
		list_iterator_start(&bin->words);
		dk = 0;
		while (list_iterator_hasnext(&bin->words))
			printd(LEVEL_EVERYTHING,		"	 0x%04X: 0x%04X\n", dk++, *(uint16_t*)list_iterator_next(&bin->words));
		list_iterator_stop(&bin->words);
		printd(LEVEL_EVERYTHING,			"	 ------: ------\n");
		if (bin->adjustment != NULL)
		{
			list_iterator_start(bin->adjustment);
			while (list_iterator_hasnext(bin->adjustment))
			{
				entry = list_iterator_next(bin->adjustment);
				assert(list_get_at(&bin->words, entry->address) != NULL);
				printd(LEVEL_EVERYTHING,	"	 0x%04X: 0x%04X (%s)\n", entry->address, *(uint16_t*)list_get_at(&bin->words, entry->address), entry->bin->data);
			}
			list_iterator_stop(bin->adjustment);
		}
		printd(LEVEL_EVERYTHING,			"	 ------: ------\n");
		if (bin->symbols != NULL)
		{
			list_iterator_start(bin->symbols);
			while (list_iterator_hasnext(bin->symbols))
				printd(LEVEL_EVERYTHING,	"	 0x%04X: DEBUG SYMBOL\n", dbgfmt_get_symbol_address(list_iterator_next(bin->symbols)));
			list_iterator_stop(bin->symbols);
		}
		printd(LEVEL_VERBOSE, "\n");
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
	size_t i, dk;

	// Create the output bin.
	target = bin_create(name);
	target->provided = list_create();
	target->required = list_create();
	target->adjustment = list_create();
	target->output = list_create();
	target->symbols = dbgfmt_create_list();

	// Loop through all of the current file bins and copy their
	// remaining (non-section) code into the output.  Since the bins
	// have already been sectioned, the remaining code in each file
	// bin will only be non-section code.
	list_iterator_start(&ldbins.bins);
	while (list_iterator_hasnext(&ldbins.bins))
	{
		bin = list_iterator_next(&ldbins.bins);

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
		printd(LEVEL_DEBUG, "moving 0x%04X words from 0x%04X in %s to 0x%04X in %s\n", list_size(&bin->words), 0, bin->name->data, list_size(&target->words), target->name->data);
		bin_move(&ldbins.bins, target, bin, list_size(&target->words), 0, list_size(&bin->words));

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

		// If there's no such output bin, that means that
		// there were no non-empty sections to fill it (because
		// the bin won't have been created during sectionization).
		if (bin == NULL)
		{
			printd(LEVEL_DEBUG, "there is no code that needs to be copied into %s; skipping\n", desired->data);
			bdestroy(desired);
			continue;
		}

		// Insert the required code.
		printd(LEVEL_DEBUG, "copying 0x%04X words from 0x%04X in %s to 0x%04X in %s\n", list_size(&bin->words), 0, bin->name->data, entry->address, target->name->data);
		bin_insert(&ldbins.bins, target, bin, entry->address, 0, list_size(&bin->words));
		bdestroy(desired);
	}
	list_iterator_stop(target->output);

	// Delete all of the bins.
	list_iterator_start(&ldbins.bins);
	while (list_iterator_hasnext(&ldbins.bins))
		bin_destroy(list_iterator_next(&ldbins.bins));
	list_iterator_stop(&ldbins.bins);
	list_clear(&ldbins.bins);

	// Add the flattened bin to the list of bins.
	list_append(&ldbins.bins, target);

	// Print result information.
	for (i = 0; i < list_size(&ldbins.bins); i++)
	{
		bin = list_get_at(&ldbins.bins, i);
		printd(LEVEL_VERBOSE, "flattened bin: %s\n", bin->name->data);
		bin->provided != NULL ? printd(LEVEL_VERBOSE,	"    total provided: %u\n", list_size(bin->provided)) : false;
		bin->required != NULL ? printd(LEVEL_VERBOSE,	"    total required: %u\n", list_size(bin->required)) : false;
		bin->adjustment != NULL ? printd(LEVEL_VERBOSE, "    total adjustment: %u\n", list_size(bin->adjustment)) : false;
		bin->section != NULL ? printd(LEVEL_VERBOSE,	"    total sections: %u\n", list_size(bin->section)) : false;
		bin->output != NULL ? printd(LEVEL_VERBOSE,	"    total outputs: %u\n", list_size(bin->output)) : false;
		bin->symbols != NULL ? printd(LEVEL_VERBOSE,	"    total symbols: %u\n", list_size(bin->symbols)) : false;
		printd(LEVEL_VERBOSE,				"    total words: 0x%04X\n", list_size(&bin->words));
		list_iterator_start(&bin->words);
		dk = 0;
		while (list_iterator_hasnext(&bin->words))
			printd(LEVEL_EVERYTHING,		"	 0x%04X: 0x%04X\n", dk++, *(uint16_t*)list_iterator_next(&bin->words));
		list_iterator_stop(&bin->words);
		printd(LEVEL_EVERYTHING,			"	 ------: ------\n");
		if (bin->adjustment != NULL)
		{
			list_iterator_start(bin->adjustment);
			while (list_iterator_hasnext(bin->adjustment))
			{
				entry = list_iterator_next(bin->adjustment);
				assert(list_get_at(&bin->words, entry->address) != NULL);
				printd(LEVEL_EVERYTHING,	"	 0x%04X: 0x%04X (%s)\n", entry->address, *(uint16_t*)list_get_at(&bin->words, entry->address), entry->bin->data);
			}
			list_iterator_stop(bin->adjustment);
		}
		printd(LEVEL_EVERYTHING,			"	 ------: ------\n");
		if (bin->symbols != NULL)
		{
			list_iterator_start(bin->symbols);
			while (list_iterator_hasnext(bin->symbols))
				printd(LEVEL_EVERYTHING,	"	 0x%04X: DEBUG SYMBOL\n", dbgfmt_get_symbol_address(list_iterator_next(bin->symbols)));
			list_iterator_stop(bin->symbols);
		}
		printd(LEVEL_VERBOSE, "\n");
	}
}

///
/// Runs each bin through the optimization infrastructure, reduce the size and
/// required number of cycles for the code.
///
/// @param mode The optimization mode.
///
void bins_optimize(int target, int level)
{
	unsigned int i;
	struct ldbin* bin;

	// Check to see whether we should skip optimizations altogether.
	if (level == OPTIMIZE_NONE)
	{
		printd(LEVEL_VERBOSE, "skipping optimizations as requested.\n");
		return;
	}

	// Loop through each bin and run the optimizers on it.
	for (i = 0; i < list_size(&ldbins.bins); i++)
	{
		bin = list_get_at(&ldbins.bins, i);
		bin_lua_optimize(bin);
	}
}

///
/// Resolves all of the required and provided labels in a program.  The bins
/// must be flattened at this point.
///
/// @param keepProvided Whether the provided label entries should be kept in the flattened
///			bin for re-exporting (for example in static libraries).
///
void bins_resolve(bool keepProvided)
{
	struct lconv_entry* entry;
	struct lconv_entry* required;
	struct lconv_entry* provided;
	struct lconv_entry* adjustment;
	struct ldbin* bin;
	uint16_t* word;
	size_t i, dk;

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


		// Add the deleted requirement as adjustment
		adjustment = malloc(sizeof(struct lconv_entry));
		if (provided->label == NULL)
			adjustment->label = NULL;
		else
		{
			adjustment->label = bfromcstr("");
			bassign(adjustment->label, provided->label);
		}

		adjustment->bin = bfromcstr("");
		bassign(adjustment->bin, bin->name);
		adjustment->address = required->address;
		if (bin->adjustment == NULL)
		{
			list_init(bin->adjustment);
		}
		list_append(bin->adjustment, adjustment);

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
		bin->provided != NULL ? printd(LEVEL_VERBOSE,	"    total provided: %u\n", list_size(bin->provided)) : false;
		bin->required != NULL ? printd(LEVEL_VERBOSE,	"    total required: %u\n", list_size(bin->required)) : false;
		bin->adjustment != NULL ? printd(LEVEL_VERBOSE, "    total adjustment: %u\n", list_size(bin->adjustment)) : false;
		bin->section != NULL ? printd(LEVEL_VERBOSE,	"    total sections: %u\n", list_size(bin->section)) : false;
		bin->output != NULL ? printd(LEVEL_VERBOSE,	"    total outputs: %u\n", list_size(bin->output)) : false;
		bin->symbols != NULL ? printd(LEVEL_VERBOSE,	"    total symbols: %u\n", list_size(bin->symbols)) : false;
		printd(LEVEL_VERBOSE,				"    total words: 0x%04X\n", list_size(&bin->words));
		list_iterator_start(&bin->words);
		dk = 0;
		while (list_iterator_hasnext(&bin->words))
			printd(LEVEL_EVERYTHING,		"	 0x%04X: 0x%04X\n", dk++, *(uint16_t*)list_iterator_next(&bin->words));
		list_iterator_stop(&bin->words);
		printd(LEVEL_EVERYTHING,			"	 ------: ------\n");
		if (bin->adjustment != NULL)
		{
			list_iterator_start(bin->adjustment);
			while (list_iterator_hasnext(bin->adjustment))
			{
				entry = list_iterator_next(bin->adjustment);
				assert(list_get_at(&bin->words, entry->address) != NULL);
				printd(LEVEL_EVERYTHING,	"	 0x%04X: 0x%04X (%s)\n", entry->address, *(uint16_t*)list_get_at(&bin->words, entry->address), entry->bin->data);
			}
			list_iterator_stop(bin->adjustment);
		}
		printd(LEVEL_EVERYTHING,			"	 ------: ------\n");
		if (bin->symbols != NULL)
		{
			list_iterator_start(bin->symbols);
			while (list_iterator_hasnext(bin->symbols))
				printd(LEVEL_EVERYTHING,	"	 0x%04X: DEBUG SYMBOL\n", dbgfmt_get_symbol_address(list_iterator_next(bin->symbols)));
			list_iterator_stop(bin->symbols);
		}
		printd(LEVEL_VERBOSE, "\n");
	}
}

///
/// Compresses all parameters into short literals if possible for
/// all of the current bins defined.
///
/// @return The amount of words that were saved in compression.
///
uint16_t bins_compress()
{
	struct ldbin* bin;
	struct lconv_entry* entry;
	size_t i, j, size;
	uint16_t* inst;
	uint16_t op, a, b, value;
	uint8_t store;
	bool apply;
	uint16_t saved = 0;

	// Iterate through all of the bins.
	list_iterator_start(&ldbins.bins);
	while (list_iterator_hasnext(&ldbins.bins))
	{
		bin = list_iterator_next(&ldbins.bins);

		// Iterate through all of the instructions in the bin, disassembling
		// each one and keeping track of how much space we save in the process.
		for (i = 0; i < list_size(&bin->words); i += size)
		{
			// Retrieve the instruction.
			inst = list_get_at(&bin->words, i);

			// Decode the instruction.
			op = INSTRUCTION_GET_OP(*inst);
			a = INSTRUCTION_GET_A(*inst);
			b = INSTRUCTION_GET_B(*inst);

			// Since B is never reduced according to the spec,
			// we have no need to handle non-basic opcodes since
			// the parameter to reduce will always be in the 'a' field.

			// Calculate total size of this instruction.
			size = 1;
			if (a == NXT_LIT || a == NXT || a == PICK)
				size++;
			if (b == NXT_LIT || b == NXT || b == PICK)
				size++;

			// Check if A operand can be reduced.
			if (a == NXT_LIT)
			{
				if (i < list_size(&bin->words))
					value = *(uint16_t*)(list_get_at(&bin->words, i + 1));
				else
					continue;

				// Check if the next word is the position
				// of a required or adjustment.
				apply = true;
				if (bin->required != NULL)
				{
					entry = NULL;
					for (j = 0; j < list_size(bin->required); j++)
					{
						entry = list_get_at(bin->required, i);
						if (i + 1 == entry->address)
						{
							apply = false;
							break;
						}
					}
				}
				if (bin->adjustment != NULL && !apply)
				{
					entry = NULL;
					for (j = 0; j < list_size(bin->adjustment); j++)
					{
						entry = list_get_at(bin->adjustment, i);
						if (i + 1 == entry->address)
						{
							apply = false;
							break;
						}
					}
				}
				if (!apply)
					continue;

				// Calculate replacement.
				if (value == 0xffff)
					store = 0x20;
				else if (value >= 0x0 && value <= 0x1e)
					store = value + 0x21;
				else
					continue;

				// Construct new instruction.
				*inst = INSTRUCTION_CREATE(op, b, store);

				// Delete unneeded word.
				assert(size > 1);
				bin_remove(&ldbins.bins, bin, i + 1, 1);
				size--;
				saved++;
			}
		}
	}
	list_iterator_stop(&ldbins.bins);

	// Return the amount of words saved.
	return saved;
}

///
/// Clears the list of bins, freeing all associated memory.
///
void bins_free()
{
	struct ldbin* bin;

	// Delete all of the bins.
	list_iterator_start(&ldbins.bins);
	while (list_iterator_hasnext(&ldbins.bins))
	{
		bin = list_iterator_next(&ldbins.bins);
		bin_destroy(bin);
	}
	list_iterator_stop(&ldbins.bins);
	list_clear(&ldbins.bins);
}
