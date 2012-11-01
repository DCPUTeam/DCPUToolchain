/**

    File:       ldbins.c

    Project:    DCPU-16 Toolchain
    Component:  Linker

    Authors:    James Rhodes

    Description:    Provides section bins for storing code in while
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
#include <derr.h>
#include "ldbins.h"
#include "ldbin.h"
#include "ldlua.h"
#include "ldconv.h"
#include "ddata.h"
#include "ldtarget.h"

///
/// Prints out information about a bin.
///
void bin_print(const char* context, struct ldbin* bin)
{
    int dk;
    struct lconv_entry* entry;
    printd(LEVEL_VERBOSE, "%s bin: %s\n", context, bin->name->data);
    bin->provided != NULL ? printd(LEVEL_VERBOSE,   "    total provided: %u\n", list_size(bin->provided)) : false;
    bin->required != NULL ? printd(LEVEL_VERBOSE,   "    total required: %u\n", list_size(bin->required)) : false;
    bin->adjustment != NULL ? printd(LEVEL_VERBOSE, "    total adjustment: %u\n", list_size(bin->adjustment)) : false;
    bin->section != NULL ? printd(LEVEL_VERBOSE,    "    total sections: %u\n", list_size(bin->section)) : false;
    bin->output != NULL ? printd(LEVEL_VERBOSE,     "    total outputs: %u\n", list_size(bin->output)) : false;
    bin->jump != NULL ? printd(LEVEL_VERBOSE,       "    total jump: %u\n", list_size(bin->jump)) : false;
    bin->optional != NULL ? printd(LEVEL_VERBOSE,   "    total optional: %u\n", list_size(bin->optional)) : false;
    bin->symbols != NULL ? printd(LEVEL_VERBOSE,    "    total symbols: %u\n", list_size(bin->symbols)) : false;
    printd(LEVEL_VERBOSE,               "    total words: 0x%04X\n", list_size(&bin->words));
    list_iterator_start(&bin->words);
    dk = 0;
    while (list_iterator_hasnext(&bin->words))
        printd(LEVEL_EVERYTHING,            "    0x%04X: 0x%04X\n", dk++, *(uint16_t*)list_iterator_next(&bin->words));
    list_iterator_stop(&bin->words);
    if (bin->adjustment != NULL)
    {
        printd(LEVEL_EVERYTHING,            "    ------: ------\n");
        printd(LEVEL_EVERYTHING,            "    ADJUSTMENTS\n");
        list_iterator_start(bin->adjustment);
        while (list_iterator_hasnext(bin->adjustment))
        {
            entry = list_iterator_next(bin->adjustment);
            assert(list_get_at(&bin->words, entry->address) != NULL);
            printd(LEVEL_EVERYTHING,            "    0x%04X: 0x%04X (%s)\n", entry->address, *(uint16_t*)list_get_at(&bin->words, entry->address), entry->bin->data);
        }
        list_iterator_stop(bin->adjustment);
    }
    if (bin->jump != NULL)
    {
        printd(LEVEL_EVERYTHING,            "    ------: ------\n");
        printd(LEVEL_EVERYTHING,            "    JUMP TABLE\n");
        list_iterator_start(bin->jump);
        while (list_iterator_hasnext(bin->jump))
        {
            entry = list_iterator_next(bin->jump);
            printd(LEVEL_EVERYTHING,            "    0x%04X (%s)\n", entry->address, entry->label->data);
        }
        list_iterator_stop(bin->jump);
    }
    printd(LEVEL_EVERYTHING,                "    ------: ------\n");
    if (bin->symbols != NULL)
    {
        list_iterator_start(bin->symbols);
        while (list_iterator_hasnext(bin->symbols))
            printd(LEVEL_EVERYTHING,            "    0x%04X: DEBUG SYMBOL\n", dbgfmt_get_symbol_address(list_iterator_next(bin->symbols)));
        list_iterator_stop(bin->symbols);
    }
    printd(LEVEL_VERBOSE, "  \n");
}

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
/// @param jump A linked list of jump table entries.
/// @param optional A linked list of optional labels.
///
struct ldbin* bins_add(freed_bstring name, struct lprov_entry* provided, struct lprov_entry* required,
                       struct lprov_entry* adjustment, struct lprov_entry* section,
                       struct lprov_entry* output, struct lprov_entry* jump,
                       struct lprov_entry* optional)
{
    struct ldbin* bin = bin_create(name);
    bin->provided = list_convert(provided);
    bin->required = list_convert(required);
    bin->adjustment = list_convert(adjustment);
    bin->section = list_convert(section);
    bin->output = list_convert(output);
    bin->jump = list_convert(jump);
    bin->optional = list_convert(optional);
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
    struct lprov_entry* jump = NULL;
    struct lprov_entry* optional = NULL;
    struct ldbin* bin;
    FILE* in;
    char* test;
    bstring sympath;
    int sympathi, sympathj, sympathk;

    // Open the input file.
    in = fopen(path.ref->data, "rb");

    if (in == NULL)
        dhalt(ERR_CAN_NOT_OPEN_FILE, path.ref->data);

    // Is this the object format?
    test = malloc(strlen(ldata_objfmt) + 1);
    memset(test, 0, strlen(ldata_objfmt) + 1);
    fread(test, 1, strlen(ldata_objfmt), in);
    fseek(in, 1, SEEK_CUR);
    if (strcmp(test, ldata_objfmt) != 0)
        dhalt(ERR_OBJECT_VERSION_MISMATCH, path.ref->data);
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
    objfile_load(path.ref->data, in, &offset, &provided, &required, &adjustment, &section, &output, &jump, &optional);

    // Add the new bin.
    bin = bins_add(path, provided, required, adjustment, section, output, jump, optional);

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
    lprov_free(jump);
    lprov_free(optional);

    return true;
}

///
/// Loads a kernel from file.
///
/// Adds a "<kernel>" bin by reading in a kernel stored on disk.  The kernel
/// file should *not* be an object file, but instead the raw data of the kernel.
/// This function must be called before loading any other bins as it does not
/// specially arrange the kernel bin to appear first in the list.
///
/// @param jumplist The path to the jumplist.
/// @param path The path to load the kernel from.
/// @return Whether the kernel bin was loaded successfully.
///
bool bins_load_kernel(freed_bstring jumplist, freed_bstring kernel)
{
    uint16_t offset = 0, store;
    struct lprov_entry* adjustment = NULL;
    struct lprov_entry* jump = NULL;
    struct lprov_entry* optional = NULL;
    struct ldbin* bin;
    FILE* jin;
    FILE* kin;
    char* test;

    // Open the input file.
    jin = fopen(jumplist.ref->data, "rb");
    if (jin == NULL)
        dhalt(ERR_CAN_NOT_OPEN_FILE, jumplist.ref->data);
    kin = fopen(kernel.ref->data, "rb");
    if (kin == NULL)
        dhalt(ERR_CAN_NOT_OPEN_FILE, kernel.ref->data);

    // Is the jumplist in the object format?
    test = malloc(strlen(ldata_objfmt) + 1);
    memset(test, 0, strlen(ldata_objfmt) + 1);
    fread(test, 1, strlen(ldata_objfmt), jin);
    fseek(jin, 1, SEEK_CUR);
    if (strcmp(test, ldata_objfmt) != 0)
        dhalt(ERR_OBJECT_VERSION_MISMATCH, jumplist.ref->data);
    free(test);

    // Load only the provided label entries into memory.
    objfile_load("<kernel>", jin, &offset, NULL, NULL, &adjustment, NULL, NULL, &jump, &optional);

    // Add the new bin.
    bin = bins_add(bautofree(bfromcstr("<kernel>")), NULL, NULL, adjustment, NULL, NULL, jump, optional);

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
        iread(&store, kin);
        if (feof(kin))
            break;
        bin_write(bin, store);
    }

    // Close the files.
    fclose(kin);
    fclose(jin);

    return true;
}

///
/// Loads a jump list from file.
///
/// Loads the jumplist references from the specified object file and uses
/// them for resolving labels not provided as part of the input objects.
///
/// @param path The path to the jumplist.
/// @result Whether the jumplist was loaded successfully.
///
bool bins_load_jumplist(freed_bstring path)
{
    uint16_t offset = 0;
    struct lprov_entry* jump = NULL;
    struct ldbin* bin;
    FILE* in;
    char* test;

    // Open the input file.
    in = fopen(path.ref->data, "rb");

    if (in == NULL)
        dhalt(ERR_CAN_NOT_OPEN_FILE, path.ref->data);

    // Is this the object format?
    test = malloc(strlen(ldata_objfmt) + 1);
    memset(test, 0, strlen(ldata_objfmt) + 1);
    fread(test, 1, strlen(ldata_objfmt), in);
    fseek(in, 1, SEEK_CUR);
    if (strcmp(test, ldata_objfmt) != 0)
        dhalt(ERR_OBJECT_VERSION_MISMATCH, path.ref->data);
    free(test);

    // Load only the provided label entries into memory.
    objfile_load(path.ref->data, in, &offset, NULL, NULL, NULL, NULL, NULL, &jump, NULL);

    // Add the new bin.
    bin = bins_add(bautofree(bfromcstr("<jumplist>")), NULL, NULL, NULL, NULL, NULL, jump, NULL);

    // Close the file.
    fclose(in);

    // Don't free the jump list here as it is used as a reference
    // in the kernel field.

    return true;
}

///
/// Saves the bin with the specified name to the specified path.
///
/// @param name The name of the bin to save.
/// @param path The path to save the bin to.
/// @param target The type of target to write.
/// @param keepOutputs Whether to keep outputs in the final image (write as object file).
/// @param symbolFilename The name of the symbol file to write out (or NULL).
/// @param jumplistFilename The name of the jump list to write out (or NULL).
///
void bins_save(freed_bstring name, freed_bstring path, int target, bool keepOutputs, const char* symbolFilename, const char* jumplistFilename)
{
    FILE* out;
    FILE* jumplist;
    struct ldbin* bin = list_seek(&ldbins.bins, name.ref);
    struct lprov_entry* provided = NULL;
    struct lprov_entry* required = NULL;
    struct lprov_entry* adjustment = NULL;
    struct lprov_entry* section = NULL;
    struct lprov_entry* outputs = NULL;
    struct lprov_entry* jump = NULL;
    struct lprov_entry* optional = NULL;
    bstring sympath;
    assert(bin != NULL);

    // Open the output file.
    out = fopen(path.ref->data, "wb");
    if (out == NULL)
    {
        bautodestroy(name);
        bautodestroy(path);
        dhalt(ERR_CAN_NOT_WRITE_FILE, path.ref->data);
    }

    // Check to see whether we need to write out
    // the linker table.
    if (target == IMAGE_STATIC_LIBRARY)
    {
        // FIXME: Free the result of list_revert (or better yet
        // convert objfmt library to use simclist).
        fwrite(ldata_objfmt, 1, strlen(ldata_objfmt) + 1, out);
        provided = list_revert(bin->provided);
        required = list_revert(bin->required);
        adjustment = list_revert(bin->adjustment);
        section = list_revert(bin->section);
        jump = list_revert(bin->jump);
        optional = list_revert(bin->optional);
        if (keepOutputs)
            outputs = list_revert(bin->output);
        objfile_save(out, provided, required, adjustment, section, outputs, jump, optional);
        lprov_free(provided);
        lprov_free(required);
        lprov_free(adjustment);
        lprov_free(section);
        lprov_free(outputs);
        lprov_free(jump);
        lprov_free(optional);
    }

    // Check if we need to just write out the jump
    // table.
    if (target == IMAGE_KERNEL && jumplistFilename != NULL)
    {
        jumplist = fopen(jumplistFilename, "wb");
        if (jumplist == NULL)
        {
            bautodestroy(name);
            bautodestroy(path);
            dhalt(ERR_CAN_NOT_WRITE_FILE, jumplistFilename);
        }
        fwrite(ldata_objfmt, 1, strlen(ldata_objfmt) + 1, jumplist);
        adjustment = list_revert(bin->adjustment);
        jump = list_revert(bin->jump);
        optional = list_revert(bin->optional);
        objfile_save(jumplist, NULL, NULL, adjustment, NULL, NULL, jump, optional);
        lprov_free(jump);
        lprov_free(optional);
        lprov_free(adjustment);
        fclose(jumplist);
    }
    else if (target == IMAGE_KERNEL)
        dwarn(WARN_KERNEL_JUMP_LIST_NOT_EXPORTED, NULL);

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
    size_t i;
    int steal, stolen, index, base;
    bstring name;

    list_init(&create);
    list_attributes_seeker(&create, &bin_seeker);

    // Print result information.
    for (i = 0; i < list_size(&ldbins.bins); i++)
    {
        bin = list_get_at(&ldbins.bins, i);
        bin_print("start", bin);
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
                target->jump = list_create();
                target->optional = list_create();
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
        bin_print("end", bin);
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
    target = bin_create(name);
    target->provided = list_create();
    target->required = list_create();
    target->adjustment = list_create();
    target->output = list_create();
    target->jump = list_create();
    target->optional = list_create();
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
        bin_print("flattened", bin);
    }
}

///
/// Writes out the jump table.
///
/// Writes out the jump table.  Bins must have been flattened at this point.
///
void bins_write_jump()
{
    bool found = false;
    uint16_t pos;
    struct ldbin* bin;
    struct ldbin* empty;
    struct lconv_entry* entry;
    struct lconv_entry* adjust;
    unsigned int i;

    // Ensure bins have been flattened.
    assert(list_size(&ldbins.bins) == 1);

    // Get flattened bin.
    bin = list_get_at(&ldbins.bins, 0);

    // If we don't have a jump list, then we don't have
    // anything to write.
    if (bin->jump == NULL)
    {
        printd(LEVEL_EVERYTHING, "no jump list in flattened bin\n");
        dwarn(WARN_KERNEL_NOT_PROVIDING_JUMP_LIST, NULL);
        return;
    }

    // Find the NULL entry if it exists.
    printd(LEVEL_EVERYTHING, "locating start of jump table...\n");
    list_iterator_start(bin->jump);
    while (list_iterator_hasnext(bin->jump))
    {
        entry = list_iterator_next(bin->jump);
        printd(LEVEL_EVERYTHING, " >> %s\n", entry->label->data);
        if (entry->label == NULL || blength(entry->label) == 0)
        {
            // Found it.
            found = true;
            pos = entry->address;
            list_iterator_stop(bin->jump);
            break;
        }
    }
    list_iterator_stop(bin->jump);

    // If we don't have a target position now, then there
    // is no jump list to write.
    if (!found)
    {
        printd(LEVEL_EVERYTHING, "no NULL entry in jump list\n");
        dwarn(WARN_KERNEL_NOT_PROVIDING_JUMP_LIST, NULL);
        return;
    }

    // Print out the bin before adding any jump entries.
    bin_print("no jump entries", bin);

    // Now loop through all of the non-empty jump list entries
    // updating references and adding words to the target bin.
    for (i = 0; i < list_size(bin->jump); i++)
    {
        entry = list_get_at(bin->jump, i);
        if (entry->label == NULL || blength(entry->label) == 0)
            continue;

        // We add 0x0000 to the jump list, not yet actually placing
        // a value in there, and then we replace it with the value
        // since by adding a new word, we'll change the position.
        empty = bin_create(bautofree(bfromcstr("<temp>")));
        empty->provided = list_create();
        empty->required = list_create();
        empty->adjustment = list_create();
        empty->output = list_create();
        empty->jump = list_create();
        empty->optional = list_create();
        empty->symbols = dbgfmt_create_list();
        bin_write(empty, 0x0000);
        bin_move(&ldbins.bins, bin, empty, pos, 0, 1);
        bin_destroy(empty);
        bin_print("after move", bin);

        // The entry address has now been adjusted by bin_move.  Set
        // the actual value.
        *(uint16_t*)(list_get_at(&bin->words, pos)) = entry->address;

        // Show information.
        printd(LEVEL_EVERYTHING, "added jump list entry at 0x%04X pointing to 0x%04X (%s)\n", pos, entry->address, entry->label->data);

        // Add an adjustment value associated with this.
        adjust = malloc(sizeof(struct lconv_entry));
        adjust->bin = bstrcpy(bin->name);
        adjust->label = NULL;
        adjust->address = pos;
        list_append(bin->adjustment, adjust);

        // Now update our jump list entry's position to point to
        // the jump table entry.
        entry->address = pos;

        // Increase the position by 1.
        pos += 1;

        // Print out the bin with jump entries.
        bin_print("jump entries", bin);
    }
}

///
/// Runs each bin through the optimization infrastructure, reduce the size and
/// required number of cycles for the code.
///
/// @param mode The optimization mode.
/// @returns The number of words saved during optimization.
///
int32_t bins_optimize(int target, int level)
{
    unsigned int i;
    struct ldbin* bin;
    int32_t saved = 0;

    // Check to see whether we should skip optimizations altogether.
    if (level == OPTIMIZE_NONE)
    {
        printd(LEVEL_VERBOSE, "skipping optimizations as requested.\n");
        return 0;
    }

    // Loop through each bin and run the optimizers on it.
    for (i = 0; i < list_size(&ldbins.bins); i++)
    {
        bin = list_get_at(&ldbins.bins, i);
        saved += bin_lua_optimize(bin);
    }

    // Return total number of words saved.
    return saved;
}

///
/// Resolves all of the required and provided labels in a program.  The bins
/// must be flattened at this point.
///
/// @param keepProvided Whether the provided label entries should be kept in the flattened
///         bin for re-exporting (for example in static libraries).
/// @param allowMissing Allow required labels to not be resolved.
/// @param keepOptional Do not resolve optional requirements and keep the linker entries.
///
void bins_resolve(bool keepProvided, bool allowMissing, bool keepOptional)
{
    struct lconv_entry* required;
    struct lconv_entry* provided;
    struct lconv_entry* adjustment;
    struct lconv_entry* jump;
    struct lconv_entry* optional;
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
        provided = bin->provided == NULL ? NULL : list_seek(bin->provided, required->label);
        jump = bin->jump == NULL ? NULL : list_seek(bin->jump, required->label);

        // Handle the entry.
        assert(required != NULL);
        if (provided == NULL && jump == NULL)
        {
            if (!allowMissing)
                dhalt(ERR_LABEL_NOT_FOUND, required->label->data);
        }
        else if (provided != NULL && jump != NULL)
            dhalt(ERR_LABEL_AMBIGIOUS_PROVIDED_JUMP, required->label->data);
        else if (provided != NULL)
        {
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
        }
        else if (jump != NULL)
        {
            // Insert the required code.
            word = list_get_at(&bin->words, required->address);
            *word = jump->address;

            // We don't add the deleted requirement as an adjustment
            // since it's an absolute reference.
        }

        adjustment->bin = bfromcstr("");
        bassign(adjustment->bin, bin->name);
        adjustment->address = required->address;
        if (bin->adjustment == NULL)
        {
            list_init(bin->adjustment);
        }
        list_append(bin->adjustment, adjustment);

        if (provided != NULL)
            printd(LEVEL_DEBUG, "resolve: %s (0x%4X) -> 0x%4X\n", required->label->data, required->address, provided->address);
        else if (jump != NULL)
            printd(LEVEL_DEBUG, "resolve: %s (0x%4X) -> 0x%4X [jump]\n", required->label->data, required->address, jump->address);
    }
    list_iterator_stop(bin->required);

    // Delete all of the required entries.
    for (i = 0; i < list_size(bin->required); i++)
    {
        required = list_extract_at(bin->required, i);
        provided = bin->provided == NULL ? NULL : list_seek(bin->provided, required->label);
        jump = bin->jump == NULL ? NULL : list_seek(bin->jump, required->label);
        if (provided == NULL && jump == NULL && allowMissing)
            continue;
        bdestroy(required->label);
        free(required);
        i--;
    }

    // Iterator over all optional values.
    list_iterator_start(bin->optional);
    while (list_iterator_hasnext(bin->optional))
    {
        // Get the optional / provided matching labels.
        optional = list_iterator_next(bin->optional);
        provided = bin->provided == NULL ? NULL : list_seek(bin->provided, optional->label);
        jump = bin->jump == NULL ? NULL : list_seek(bin->jump, optional->label);

        // This optional entry could refer to an address that is
        // not specified (for example in the case of a jumplist and
        // kernel).
        if (optional->address > list_size(&bin->words))
            continue;

        // Handle the entry.
        assert(optional != NULL);
        if (provided == NULL && jump == NULL)
        {
            // Insert the optional code.
            word = list_get_at(&bin->words, optional->address);
            *word = 0x0000;
        }
        else if (provided != NULL && jump != NULL)
            dhalt(ERR_LABEL_AMBIGIOUS_PROVIDED_JUMP, optional->label->data);
        else if (provided != NULL)
        {
            // Insert the optional code.
            word = list_get_at(&bin->words, optional->address);
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
        }
        else if (jump != NULL)
        {
            // Insert the optional code.
            word = list_get_at(&bin->words, optional->address);
            *word = jump->address;

            // We don't add the deleted requirement as an adjustment
            // since it's an absolute reference.
        }

        adjustment->bin = bfromcstr("");
        bassign(adjustment->bin, bin->name);
        adjustment->address = optional->address;
        if (bin->adjustment == NULL)
        {
            list_init(bin->adjustment);
        }
        list_append(bin->adjustment, adjustment);

        if (provided != NULL)
            printd(LEVEL_DEBUG, "resolve: %s (0x%4X) -> 0x%4X\n", optional->label->data, optional->address, provided->address);
        else if (jump != NULL)
            printd(LEVEL_DEBUG, "resolve: %s (0x%4X) -> 0x%4X [jump]\n", optional->label->data, optional->address, jump->address);
        else
            printd(LEVEL_DEBUG, "resolve: %s (0x%4X) -> 0x0000 [not found]\n", optional->label->data, optional->address);
    }
    list_iterator_stop(bin->optional);

    if (!keepOptional)
    {
        // Delete all of the optional entries.
        for (i = 0; i < list_size(bin->optional); i++)
        {
            optional = list_extract_at(bin->optional, i);
            provided = bin->provided == NULL ? NULL : list_seek(bin->provided, optional->label);
            jump = bin->jump == NULL ? NULL : list_seek(bin->jump, optional->label);
            if (provided == NULL && jump == NULL && allowMissing)
                continue;
            bdestroy(optional->label);
            free(optional);
            i--;
        }
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
        bin_print("resolved", bin);
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
                        entry = list_get_at(bin->required, j);
                        if (i + 1 == entry->address)
                        {
                            apply = false;
                            break;
                        }
                    }
                }
                if (bin->optional != NULL && apply)
                {
                    entry = NULL;
                    for (j = 0; j < list_size(bin->optional); j++)
                    {
                        entry = list_get_at(bin->optional, j);
                        if (i + 1 == entry->address)
                        {
                            apply = false;
                            break;
                        }
                    }
                }
                if (bin->adjustment != NULL && apply)
                {
                    entry = NULL;
                    for (j = 0; j < list_size(bin->adjustment); j++)
                    {
                        entry = list_get_at(bin->adjustment, j);
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
