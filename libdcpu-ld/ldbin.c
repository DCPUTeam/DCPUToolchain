/**

    File:       ldbin.c

    Project:    DCPU-16 Toolchain
    Component:  Linker

    Authors:    James Rhodes

    Description:    Provides section bins for storing code in while
            section and output dependencies are resolved.

**/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <debug.h>
#include <derr.h>
#include "ldbin.h"
#include "ldconv.h"
#include "ddata.h"

///
/// Creates a new linker bin.
///
/// @param name The name of the linker bin.
/// @param initialize Whether to initialize the list structures.
/// @return The new linker bin.
///
struct ldbin* bin_create(freed_bstring name, bool initialize)
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
    bin->jump = NULL;
    bin->optional = NULL;
    bin->call = NULL;
    bin->symbols = NULL;
    if (initialize)
    {
        bin->provided = list_create();
        bin->required = list_create();
        bin->adjustment = list_create();
        bin->section = list_create();
        bin->output = list_create();
        bin->jump = list_create();
        bin->optional = list_create();
        bin->call = list_create();
        bin->symbols = dbgfmt_create_list();
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
    if (bin->provided != NULL) list_prov_destroy(bin->provided);
    if (bin->required != NULL) list_prov_destroy(bin->required);
    if (bin->adjustment != NULL) list_prov_destroy(bin->adjustment);
    if (bin->section != NULL) list_prov_destroy(bin->section);
    if (bin->output != NULL) list_prov_destroy(bin->output);
    if (bin->jump != NULL) list_prov_destroy(bin->jump);
    if (bin->optional != NULL) list_prov_destroy(bin->optional);
    if (bin->call != NULL) list_prov_destroy(bin->call);
    if (bin->symbols != NULL) dbgfmt_free(bin->symbols);
    list_destroy(&bin->words);
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
/// offset and transferring up to count words.  Linker information
/// is automatically adjusted as part of this process.
///
/// @param all The list of all current bins.
/// @param to The target bin to write the words into.
/// @param from The source bin to read words from.
/// @param at The address in the target bin where writing should occur.
/// @param offset The address in the source bin where reading should occur.
/// @param count The number of words to be moved.
///
void bin_move(list_t* all, struct ldbin* to, struct ldbin* from, size_t at, size_t offset, size_t count)
{
    bin_insert(all, to, from, at, offset, count);
    bin_remove(all, from, offset, count);
}

///
/// Replaces words in one bin with another, adjusting as needed.
///
/// Replaces words in one bin with another, starting at the specified
/// offset and transferring up to count words.  Linker information
/// is automatically adjusted as part of this process.
///
/// @param all The list of all current bins.
/// @param to The target bin to write the words into.
/// @param from The source bin to read words from.
/// @param at The address in the target bin where writing should occur.
/// @param erase The number of words to be discarded before writing.
/// @param offset The address in the source bin where reading should occur.
/// @param count The number of words to be written.
///
void bin_replace(list_t* all, struct ldbin* to, struct ldbin* from, size_t at, size_t erase, size_t offset, size_t count)
{
    bin_remove(all, to, at, erase);
    bin_insert(all, to, from, at, offset, count);
}

///
/// Appends words from one bin to another, adjusting as needed.
///
/// Appends words from one bin to another, starting at the specified
/// offset and transferring up to count words.  Linker information
/// is automatically adjusted as part of this process.
///
/// @param all The list of all current bins.
/// @param to The target bin to write the words into.
/// @param from The source bin to read words from.
/// @param offset The address in the source bin where reading should occur.
/// @param count The number of words to be written.
///
void bin_append(list_t* all, struct ldbin* to, struct ldbin* from, size_t offset, size_t count)
{
    bin_insert(all, to, from, list_size(&to->words), offset, count);
}

///
/// Inserts words from one bin to another, adjusting as needed.
///
/// Inserts words from one bin to another, starting at the specified
/// offset and transferring up to count words.  Linker information
/// is automatically adjusted as part of this process.
///
/// @param all The list of all current bins.
/// @param to The target bin to write the words into.
/// @param from The source bin to read words from.
/// @param at The address in the target bin where writing should occur.
/// @param offset The address in the source bin where reading should occur.
/// @param count The number of words to be written.
///
void bin_insert(list_t* all, struct ldbin* to, struct ldbin* from, size_t at, size_t offset, size_t count)
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
    bin_info_insert(all, to, to->provided, from, from->provided, false, false, at, offset, count);
    bin_info_insert(all, to, to->required, from, from->required, false, false, at, offset, count);
    bin_info_insert(all, to, to->adjustment, from, from->adjustment, true, false, at, offset, count);
    bin_info_insert(all, to, to->output, from, from->output, false, true, at, offset, count);
    bin_info_insert(all, to, to->jump, from, from->jump, false, false, at, offset, count);
    bin_info_insert(all, to, to->optional, from, from->optional, false, false, at, offset, count);
    bin_info_insert(all, to, to->call, from, from->call, false, false, at, offset, count);
    bin_info_insert_symbols(to, from, at, offset, count);
}

///
/// Removes words in a bin.
///
/// Removed words in a bin, starting at the specified
/// offset and transferring up to count words.  Linker information
/// is automatically removed as part of this process.
///
/// @param all The list of all current bins.
/// @param bin The bin to remove words from.
/// @param offset The address to start removal from.
/// @param count The number of words to be removed.
///
void bin_remove(list_t* all, struct ldbin* bin, size_t offset, size_t count)
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
    bin_info_remove(all, bin, bin->provided, false, offset, count);
    bin_info_remove(all, bin, bin->required, false, offset, count);
    bin_info_remove(all, bin, bin->adjustment, true, offset, count);
    bin_info_remove(all, bin, bin->output, false, offset, count);
    bin_info_remove(all, bin, bin->jump, false, offset, count);
    bin_info_remove(all, bin, bin->optional, false, offset, count);
    bin_info_remove(all, bin, bin->call, false, offset, count);
    bin_info_remove_symbols(bin, offset, count);
}

extern long allocated_strings;

///
/// Copies references from the specified linker information list to another linker information list.
///
/// @param all The list of all current bins.
/// @param to The target bin that words were inserted into.
/// @param tolist The target linker information list to adjust.
/// @param from The source bin that words were read from.
/// @param fromlist The source linker information list to adjust.
/// @param at The address in the target bin where the insert occurred.
/// @param offset The address in the source bin where words were copied from.
/// @param count The number of words that were inserted.
///
void bin_info_insert(list_t* all, struct ldbin* to, list_t* tolist, struct ldbin* from, list_t* fromlist, bool isAdjustment, bool isOutput, size_t at, size_t offset, size_t count)
{
    struct ldbin* abin;
    struct lconv_entry* entry;
    struct lconv_entry* copy;
    size_t k, j;
    uint16_t* word;

    // Skip if the from list is NULL.
    if (fromlist == NULL) return;
    assert(tolist != NULL);

    // Sort the list by addresses.
    list_sort(fromlist, 1);

    if (!isAdjustment)
    {
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

                if (entry->address >= at)
                {
                    // Shift the address up.
                    entry->address += count;
                }
            }
        }

        // Copy all of the new linker information entries
        // across into the correct place.
        for (k = 0; k < list_size(fromlist); k++)
        {
            entry = (struct lconv_entry*)list_get_at(fromlist, k);

            if (entry->address >= offset && (entry->address < offset + count || (entry->address >= list_size(&from->words) && offset + count + 1 >= list_size(&from->words))))
            {
                // Copy this linker information entry.
                copy = malloc(sizeof(struct lconv_entry));
                copy->address = entry->address;
                copy->bin = entry->bin;
                if (entry->label == NULL)
                    copy->label = NULL;
                else
                {
                    copy->label = bfromcstr("");
                    bassign(copy->label, entry->label);
                }

                // Adjust the copy.
                copy->address = at + (entry->address - offset);
                copy->bin = entry->bin;
                list_append(tolist, copy);

                // Free copy (list automatically copies data for us).
                free(copy);
            }
        }
    }
    // If this is the adjustment list, then go through all of the bins
    // and make appropriate changes.
    else
    {
        // adjust all the adjustments past the insertion point
        for (k = 0; k < list_size(tolist); k++)
        {
            entry = (struct lconv_entry*)list_get_at(tolist, k);
            if (entry->address >= at)
            {
                entry->address += count;
            }
        }

        // Copy all of the new linker information entries
        // across into the correct place.
        for (k = 0; k < list_size(fromlist); k++)
        {
            entry = (struct lconv_entry*)list_get_at(fromlist, k);

            if (entry->address >= offset && (entry->address < offset + count || (entry->address >= list_size(&from->words) && offset + count + 1 >= list_size(&from->words))))
            {
                // Copy this linker information entry.
                copy = malloc(sizeof(struct lconv_entry));
                copy->address = entry->address;
                copy->bin = entry->bin;
                if (entry->label == NULL)
                    copy->label = NULL;
                else
                {
                    copy->label = bfromcstr("");
                    bassign(copy->label, entry->label);
                }

                // Adjust the copy.
                copy->address = at + (entry->address - offset);
                copy->bin = entry->bin;
                list_append(tolist, copy);

                // Free copy (list automatically copies data for us).
                free(copy);
            }
        }

        // Loop through all of the bins.
        for (k = 0; k < list_size(all); k++)
        {
            abin = list_get_at(all, k);
            if (abin->adjustment == NULL) continue;

            // Skip the target bin since we'll handle that outside
            // the loop (the reason for this is that in some cases, such
            // as flattening, the target bin isn't yet in the list of bins
            // yet, but we still need to handle it).
            if (abin == to)
                continue;

            // Loop through all of the adjustments and modify them
            // if they are targetting the from bin.
            for (j = 0; j < list_size(abin->adjustment); j++)
            {
                entry = (struct lconv_entry*)list_get_at(abin->adjustment, j);

                // Get the existing value of the adjustment.
                word = (uint16_t*)list_get_at(&abin->words, entry->address);
                assert(word != NULL);

                // Check to see if this adjustment entry is pointing into
                // the old bin.
                if (*word >= offset && *word < offset + count && biseq(entry->bin, from->name))
                {
                    printd(LEVEL_EVERYTHING, "repointing internal adjustment to from[offset,offset+count) at (0x%04X, %s) from (0x%04X, %s) to (0x%04X, %s)\n",
                           entry->address, abin->name->data,
                           *word, entry->bin->data,
                           *word - offset + at, to->name->data);

                    *word = *word - offset + at;
                    entry->bin = to->name;
                }
                // adjustment was pointing into the new bin
                else if (*word >= at && biseq(entry->bin, to->name))
                {
                    printd(LEVEL_EVERYTHING, "repointing (to) internal adjustment in to[at+count) at (0x%04X, %s) from (0x%04X, %s) to (0x%04X, %s)\n",
                           entry->address, abin->name->data,
                           *word, entry->bin->data,
                           *word - offset + count, to->name->data);

                    *word = *word + count;
                }
            }
        }
        abin = NULL;

        // Loop through all of the adjustments and modify them
        // if they are targetting the from bin.
        for (j = 0; j < list_size(to->adjustment); j++)
        {
            entry = (struct lconv_entry*)list_get_at(to->adjustment, j);

            // Get the existing value of the adjustment.
            word = (uint16_t*)list_get_at(&to->words, entry->address);
            assert(word != NULL);

            // Check to see if this adjustment entry is pointing into
            // the old bin.
            if (*word >= offset && *word < offset + count && biseq(entry->bin, from->name))
            {
                printd(LEVEL_EVERYTHING, "repointing internal adjustment to from[offset,offset+count) at (0x%04X, %s) from (0x%04X, %s) to (0x%04X, %s)\n",
                       entry->address, to->name->data,
                       *word, entry->bin->data,
                       *word - offset + at, to->name->data);

                *word = *word - offset + at;
                entry->bin = to->name;
            }
            // internal adjustments (internal to 'to')
            else if (*word >= at && biseq(entry->bin, to->name))
            {
                printd(LEVEL_EVERYTHING, "repointing internal adjustment to to[at,...) at (0x%04X, %s) from (0x%04X, %s) to (0x%04X, %s)\n",
                       entry->address, to->name->data,
                       *word, entry->bin->data,
                       *word + count, to->name->data);

                *word = *word + count;
                //entry->bin = to->name;
            }
        }
    }
}

///
/// Removes references in the specified linker information list.
///
/// @param all The list of all current bins.
/// @param bin The bin that this linker information list belongs to.
/// @param list The linker information list to adjust.
/// @param isAdjustment Whether the linker information list is an adjustment list.
/// @param offset The address that words were removed from.
/// @param count The number of words that were removed.
///
void bin_info_remove(list_t* all, struct ldbin* bin, list_t* list, bool isAdjustment, size_t offset, size_t count)
{
    struct lconv_entry* entry;
    struct ldbin* abin;
    size_t k, j;
    uint16_t* word;

    // Skip if the list is NULL.
    if (list == NULL) return;

    // Sort the list by addresses.
    list_sort(list, 1);

    // Go through all of the information entries in the list.
    if (!isAdjustment)
    {
        for (k = 0; k < list_size(list); k++)
        {
            entry = (struct lconv_entry*)list_get_at(list, k);

            if (entry->address < offset)
                // Prior to the current address, so don't touch it.
                continue;
            else if (entry->address >= offset && entry->address < offset + count)
            {
                // Remove this linker information entry.
                bdestroy(entry->label);
                assert(list_delete_at(list, k) == 0);
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
    else
    {
        // adjust adjustment addresses
        for (k = 0; k < list_size(list); k++)
        {
            entry = (struct lconv_entry*)list_get_at(list, k);

            if (entry->address < offset)
                // Prior to the current address, so don't touch it.
                continue;
            else if (entry->address >= offset && entry->address < offset + count)
            {
                // Remove this linker information entry.
                bdestroy(entry->label);
                assert(list_delete_at(list, k) == 0);
                k--;
            }
            else if (entry->address >= offset + count)
            {
                // Adjust the address stored in the entry down
                // by the amount of words that were removed.
                // FIXME: This causes silent corruption (see issue #191).
                //entry->address -= count;
            }
        }

        // Loop through all of the bins.
        for (k = 0; k < list_size(all); k++)
        {
            abin = list_get_at(all, k);
            if (abin->adjustment == NULL) continue;

            // Loop through all of the adjustments and modify them
            // if they are targetting the from bin.
            for (j = 0; j < list_size(abin->adjustment); j++)
            {
                entry = (struct lconv_entry*)list_get_at(abin->adjustment, j);

                if (entry->address >= offset + count && biseq(entry->bin, bin->name))
                {
                    entry->address -= count;
                }

                // Get the existing value of the adjustment.
                word = (uint16_t*)list_get_at(&abin->words, entry->address);
                if (word == NULL)
                    dhalt(ERR_BUG_191_WORKAROUND, NULL);
                assert(word != NULL);

                // Check to see if this adjustment entry is pointing into
                // the old bin.
                if (*word >= offset + count && biseq(entry->bin, bin->name))
                {
                    printd(LEVEL_EVERYTHING, "repointing (to) internal adjustment to from[offset+count,...) at (0x%04X, %s) from (0x%04X, %s) to (0x%04X, %s)\n",
                           entry->address, abin->name->data,
                           *word, entry->bin->data,
                           *word - count, bin->name->data);

                    *word = *word - count;
                }
            }
        }
    }
}

///
/// Copies debugging symbols from one bin to another.
///
/// @param to The target bin that words were inserted into.
/// @param from The source bin that words were read from.
/// @param at The address in the target bin where the insert occurred.
/// @param offset The address in the source bin where words were copied from.
/// @param count The number of words that were inserted.
///
void bin_info_insert_symbols(struct ldbin* to, struct ldbin* from, size_t at, size_t offset, size_t count)
{
    struct dbg_sym* sym;
    struct dbg_sym* copy;
    size_t k;
    uint16_t address;

    // Skip if the from list is NULL.
    if (from->symbols == NULL) return;
    assert(to->symbols != NULL);

    // Adjust all of the memory addresses for debugging symbol entries in the
    // bin that was written to and shift them up.
    for (k = 0; k < list_size(to->symbols); k++)
    {
        sym = (struct dbg_sym*)list_get_at(to->symbols, k);
        address = dbgfmt_get_symbol_address(sym);

        if (address >= at)
        {
            // Shift the address up.
            dbgfmt_update_symbol(sym, address + count);
        }
    }

    // Copy all of the new debugging symbol entries
    // across into the correct place.
    for (k = 0; k < list_size(from->symbols); k++)
    {
        sym = (struct dbg_sym*)list_get_at(from->symbols, k);
        address = dbgfmt_get_symbol_address(sym);

        if (address >= offset && (address < offset + count || (address >= list_size(&from->words) && offset + count + 1 >= list_size(&from->words))))
        {
            // Copy this debugging symbol entry.
            copy = dbgfmt_copy_symbol(sym);

            // Adjust the copy.
            dbgfmt_update_symbol(copy, at + (address - offset));
            list_append(to->symbols, copy);
            free(copy);
        }
    }
}

///
/// Removes debugging symbols in the specified linker bin.
///
/// @param bin The bin that this linker information list belongs to.
/// @param offset The address that words were removed from.
/// @param count The number of words that were removed.
///
void bin_info_remove_symbols(struct ldbin* bin, size_t offset, size_t count)
{
    struct dbg_sym* sym;
    size_t k;
    uint16_t address;

    // Sort the list by addresses.
    list_sort(bin->symbols, 1);

    // Go through all of the information entries in the list.
    for (k = 0; k < list_size(bin->symbols); k++)
    {
        sym = (struct dbg_sym*)list_get_at(bin->symbols, k);
        address = dbgfmt_get_symbol_address(sym);

        if (address < offset)
            // Prior to the current address, so don't touch it.
            continue;
        else if (address >= offset && address < offset + count)
        {
            // Remove this linker information entry.
            dbgfmt_free_symbol(sym);
            assert(list_delete_at(bin->symbols, k) == 0);
            k--;
        }
        else if (address >= offset + count)
        {
            // Adjust the address stored in the entry down
            // by the amount of words that were removed.
            dbgfmt_update_symbol(sym, address - count);
        }
    }
}
