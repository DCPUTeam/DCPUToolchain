/**

    File:       textn.c

    Project:    DCPU-16 Tools
    Component:  Assembler

    Authors:    James Rhodes

    Description:    Defines public functions for writing out
            extension tables.

**/

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <debug.h>
#include <derr.h>
#include <iio.h>
#include "posix.h"
#include "textn.h"
#include "aout.h"

char* extension_data[EXTENSION_MAXIMUM_ENTRIES];
uint16_t extension_count = 0;
uint16_t extension_offset = 0;

uint16_t textn_init(struct aout_byte* start)
{
    struct aout_byte* current;
    uint32_t mem_index;
    char* ext_name;

    // Write out the table (maximum 2000 entries).
    extension_count = 0;
    extension_offset = 0;
    current = start;
    mem_index = 0;

    while (current != NULL)
    {
        if (current->type == AOUT_TYPE_METADATA_EXTENSION)
        {
            ext_name = textn_verify_name(current->label);

            if (ext_name == NULL)
                dhalt(ERR_EXTENSION_UNKNOWN, current->label);

            extension_data[extension_count] = ext_name;
            printd(LEVEL_VERBOSE, "EXTENSION [0x%04X] %s\n", extension_offset, ext_name);
            extension_count += 1;
            extension_offset += strlen(ext_name) + 1;

            if (extension_count == EXTENSION_MAXIMUM_ENTRIES)
                dhalt(ERR_EXTENSION_TABLE_TOO_LARGE, NULL);
        }

        current = current->next;
    }

    // Return how much code should be offset by.
    if (extension_count > 0)
        return EXTENSION_OFFSET;
    else
        return 0;
}

void textn_write(FILE* out)
{
    uint16_t i, inst, a;

    // If they haven't declared any extensions, we don't
    // output the table at all.
    if (extension_count == 0)
        return;

    EXTENSION_WRITE_OP(OP_SET, PC, NXT_LIT);
    EXTENSION_WRITE_RAW(EXTENSION_OFFSET);
    EXTENSION_WRITE_RAW(EXTENSION_MAGIC);
    EXTENSION_WRITE_RAW(EXTENSION_VERSION);
    EXTENSION_WRITE_RAW(extension_count);

    for (i = 0; i < extension_count; i += 1)
    {
        for (a = 0; a < strlen(extension_data[i]); a += 1)
        {
            EXTENSION_WRITE_RAW(extension_data[i][a]);
        }

        EXTENSION_WRITE_RAW(0);
    }
}

#define EXTENSION_MAP_MAXIMUM 3

char* extension_map[EXTENSION_MAP_MAXIMUM] =
{
    // The names are important here, don't change them!
    "experimental-output-screen-32x12-0x8000",
    "experimental-output-screen-32x16-0x8000",
    "experimental-input-keyboard-single-0x9000"
};

char* textn_verify_name(char* name)
{
    uint16_t i;

    for (i = 0; i < EXTENSION_MAP_MAXIMUM; i += 1)
    {
        if (stricmp(name, extension_map[i]) == 0)
            return extension_map[i];
    }

    return NULL;
}
