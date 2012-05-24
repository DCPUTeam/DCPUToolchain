/**

	File:		ldbin.c

	Project:	DCPU-16 Toolchain
	Component:	Linker

	Authors:	James Rhodes

	Description:	Provides section bins for storing code in while
			section and output dependencies are resolved.

**/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "ldbin.h"
#include "ldconv.h"

///
/// Creates a new linker bin.
///
/// @param name The name of the linker bin.
/// @return The new linker bin.
///
struct ldbin* bin_create(freed_bstring name, bool initLists)
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
	if (initLists)
	{
		bin->provided = list_create();
		bin->required = list_create();
		bin->adjustment = list_create();
		bin->section = list_create();
		bin->output = list_create();
	}
	else
	{
		bin->provided = NULL;
		bin->required = NULL;
		bin->adjustment = NULL;
		bin->section = NULL;
		bin->output = NULL;
	}
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

///
/// Moves words from one bin to another, adjusting as needed.
///
/// Moves words from one bin to another, starting at the specified
/// offset and transferring up to count words.	Linker information
/// is automatically adjusted as part of this process.
///
/// @param to The target bin to write the words into.
/// @param from The source bin to read words from.
/// @param at The address in the target bin where writing should occur.
/// @param offset The address in the source bin where reading should occur.
/// @param count The number of words to be moved.
///
void bin_move(struct ldbin* to, struct ldbin* from, size_t at, size_t offset, size_t count)
{
	bin_insert(to, from, at, offset, count);
	bin_remove(from, offset, count);
}

///
/// Replaces words in one bin with another, adjusting as needed.
///
/// Replaces words in one bin with another, starting at the specified
/// offset and transferring up to count words.	Linker information
/// is automatically adjusted as part of this process.
///
/// @param to The target bin to write the words into.
/// @param from The source bin to read words from.
/// @param at The address in the target bin where writing should occur.
/// @param erase The number of words to be discarded before writing.
/// @param offset The address in the source bin where reading should occur.
/// @param count The number of words to be written.
///
void bin_replace(struct ldbin* to, struct ldbin* from, size_t at, size_t erase, size_t offset, size_t count)
{
	bin_remove(to, at, erase);
	bin_insert(to, from, at, offset, count);
}

///
/// Appends words from one bin to another, adjusting as needed.
///
/// Appends words from one bin to another, starting at the specified
/// offset and transferring up to count words.	Linker information
/// is automatically adjusted as part of this process.
///
/// @param to The target bin to write the words into.
/// @param from The source bin to read words from.
/// @param offset The address in the source bin where reading should occur.
/// @param count The number of words to be written.
///
void bin_append(struct ldbin* to, struct ldbin* from, size_t offset, size_t count)
{
	bin_insert(to, from, list_size(&to->words), offset, count);
}

///
/// Inserts words from one bin to another, adjusting as needed.
///
/// Inserts words from one bin to another, starting at the specified
/// offset and transferring up to count words.	Linker information
/// is automatically adjusted as part of this process.
///
/// @param to The target bin to write the words into.
/// @param from The source bin to read words from.
/// @param at The address in the target bin where writing should occur.
/// @param offset The address in the source bin where reading should occur.
/// @param count The number of words to be written.
///
void bin_insert(struct ldbin* to, struct ldbin* from, size_t at, size_t offset, size_t count)
{
	size_t i;

	assert(to != NULL);
	assert(from != NULL);
	assert(at >= 0 && at <= list_size(&to->words));
	assert(offset >= 0 && offset <= list_size(&from->words));
	assert(offset + count >= 0 && offset + count <= list_size(&from->words));

	for (i = 0; i < count; i++)
	{
		// Copy the word.
		list_insert_at(&to->words, list_get_at(&from->words, offset + i), at + i);
	}

	// Perform linker information updates.
	bin_info_insert(to, to->provided, from, from->provided, false, false, at, offset, count);
	bin_info_insert(to, to->required, from, from->required, false, false, at, offset, count);
	bin_info_insert(to, to->adjustment, from, from->adjustment, true, false, at, offset, count);
	bin_info_insert(to, to->output, from, from->output, false, true, at, offset, count);
}

///
/// Removes words in a bin.
///
/// Removed words in a bin, starting at the specified
/// offset and transferring up to count words.	Linker information
/// is automatically removed as part of this process.
///
/// @param bin The bin to remove words from.
/// @param offset The address to start removal from.
/// @param count The number of words to be removed.
///
void bin_remove(struct ldbin* bin, size_t offset, size_t count)
{
	size_t i;

	assert(bin != NULL);
	assert(offset >= 0 && offset <= list_size(&bin->words));
	assert(offset + count >= 0 && offset + count <= list_size(&bin->words));

	for (i = 0; i < count; i++)
	{
		// Delete the word.
		list_delete_at(&bin->words, offset);
	}

	// Perform linker information updates.
	bin_info_remove(bin, bin->provided, offset, count);
	bin_info_remove(bin, bin->required, offset, count);
	bin_info_remove(bin, bin->adjustment, offset, count);
	bin_info_remove(bin, bin->output, offset, count);
}

///
/// Copies references from the specified linker information list to another linker information list.
///
/// @param to The target bin that words were inserted into.
/// @param tolist The target linker information list to adjust.
/// @param from The source bin that words were read from.
/// @param fromlist The source linker information list to adjust.
/// @param at The address in the target bin where the insert occurred.
/// @param offset The address in the source bin where words were copied from.
/// @param count The number of words that were inserted.
///
void bin_info_insert(struct ldbin* to, list_t* tolist, struct ldbin* from, list_t* fromlist, bool isAdjustment, bool isOutput, size_t at, size_t offset, size_t count)
{
	struct lconv_entry* entry;
	struct lconv_entry* copy;
	size_t k;
	uint16_t* word;

	// Skip if the from list is NULL.
	if (fromlist == NULL) return;
	assert(tolist != NULL);

	// Sort the list by addresses.
	list_sort(fromlist, 1);

	// Skip shifting if this is the output linker info list because
	// that causes the output addresses to be shifted up when we're
	// flattening bins.
	if (!isOutput)
	{
		// Adjust all of the memory addresses for linker entries in the
		// bin that was written to and shift them up.
		for (k = 0; k < list_size(tolist); k++)
		{
			entry = (struct lconv_entry*)list_get_at(tolist, k);

			if (entry->address >= offset + count)
			{
				// Shift the address up.
				entry->address += count;

				// If this is an adjustment entry, we need to adjust it
				// immediately (since we have no label to keep track of).
				if (isAdjustment)
				{
					word = (uint16_t*)list_get_at(&to->words, entry->address);
					if (*word >= offset)
						*word = *word + count;
				}
			}
		}
	}

	// Copy all of the new linker information entries
	// across into the correct place.
	for (k = 0; k < list_size(fromlist); k++)
	{
		entry = (struct lconv_entry*)list_get_at(fromlist, k);

		if (entry->address >= offset && entry->address < offset + count)
		{
			// Copy this linker information entry.
			copy = malloc(sizeof(struct lconv_entry));
			copy->address = entry->address;
			copy->label = bfromcstr("");
			bassign(copy->label, entry->label);

			// Adjust the copy.
			copy->address = at + (entry->address - offset);
			list_append(tolist, copy);

			// If this is an adjustment entry, we need to adjust it
			// immediately (since we have no label to keep track of).
			if (isAdjustment)
			{
				word = (uint16_t*)list_get_at(&to->words, copy->address);
				// TODO: Proper error message for when adjustment values are
				//	 currently outside the code that's being copied across.
				assert(*word >= offset && *word < offset + count);
				*word = *word + (copy->address - entry->address);
			}
		}
	}
}

///
/// Removes references in the specified linker information list.
///
/// @param bin The bin that this linker information list belongs to.
/// @param list The linker information list to adjust.
/// @param isAdjustment Whether the linker information list is an adjustment list.
/// @param offset The address that words were removed from.
/// @param count The number of words that were removed.
///
void bin_info_remove(struct ldbin* bin, list_t* list, size_t offset, size_t count)
{
	struct lconv_entry* entry;
	size_t k;

	// Skip if the list is NULL.
	if (list == NULL) return;

	// Sort the list by addresses.
	list_sort(list, 1);

	// Go through all of the information entries in the list.
	for (k = 0; k < list_size(list); k++)
	{
		entry = (struct lconv_entry*)list_get_at(list, k);

		if (entry->address < offset)
			// Prior to the current address, so don't touch it.
			continue;
		else if (entry->address >= offset && entry->address < offset + count)
		{
			// Remove this linker information entry.
			list_delete_at(list, k);
			k--;
		}
		else if (entry->address >= offset + count)
		{
			// Adjust the address stored in the entry down
			// by the amount of words that were removed.
			entry->address -= count;
		}
	}
}