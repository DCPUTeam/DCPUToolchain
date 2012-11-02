/**

    File:           ppcompat.c

    Project:        DCPU-16 Toolchain
    Component:      LibDCPU-PP

    Authors:        James Rhodes

    Description:    Defines the compatibility API for
                    programs using the old preprocessor API.

**/

#include <stdio.h>
#include <bstring.h>
#include <bfile.h>
#include <derr.h>
#include "ppcompat.h"
#include "pp.h"

static BFILE* _in_file = NULL;
static BFILE* _out_file = NULL;

static bool _has_input()
{
    return !bfeof(_in_file);
}

static char _input()
{
    char c = (char)bfgetc(_in_file);
    return c;
}

static void _output(char c)
{
    bfputc(c, _out_file);
}

bstring pp_do(freed_bstring lang, freed_bstring path)
{
    bstring temp;
    freed_bstring friendly;
    int parse_error;
    bstring leadin = NULL;

    // Open and set up the temporary output areas.
    temp = bfromcstr(tempnam(".", "pp."));

    if (biseq(path.ref, bfromcstr("-")))
    {
        friendly = bautofree(bfromcstr("-"));
        _in_file = bfwrap(stdin, "r");
    }
    else
    {
        friendly = bautofree(bstrcpy(path.ref));
        _in_file = bfopen((const char*)(path.ref->data), "r");
    }

    if (_in_file == NULL)
    {
        dhalt(ERR_PP_COMPAT_OPEN_INPUT_FAILED, NULL);
        return NULL;
    }

    _out_file = bfopen((const char*)(temp->data), "w");

    if (_out_file == NULL)
    {
        if (!biseq(path.ref, bfromcstr("-")))
            bfclose(_in_file);

        dhalt(ERR_PP_COMPAT_OPEN_TEMP_FAILED, NULL);
        return NULL;
    }

    // Output the initial line information depending on language.
    if (biseqcstrcaseless(lang.ref, "asm"))
    {
        if (biseq(path.ref, bfromcstr("-")))
            leadin = bformat(".LINE 1 \"<stdin>\"\n");
        else
            leadin = bformat(".LINE 1 \"%s\"\n", path.ref->data);
    }
    else if (biseqcstrcaseless(lang.ref, "c"))
    {
        if (biseq(path.ref, bfromcstr("-")))
            leadin = bformat("#line 1 \"<stdin>\"\n");
        else
            leadin = bformat("#line 1 \"%s\"\n", path.ref->data);
    }
    if (leadin != NULL)
        bfwrite(leadin->data, 0, blength(leadin), _out_file);

    // Now invoke the preprocessor's lexer and parser.
    ppimpl(lang, _has_input, _input, _output);

    // Now do some cleanup.
    if (!biseq(path.ref, bfromcstr("-")))
        bfclose(_in_file);

    // Close the output file.
    bfclose(_out_file);

    // Free bstring.
    bautodestroy(path);

    // Return the filename.
    return temp;
}

void pp_cleanup(freed_bstring path)
{
    if (path.ref == NULL) return;

    remove((const char*)(path.ref->data));
    bautodestroy(path);
}
