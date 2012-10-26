/**

    File:       pp.c

    Project:    DCPU-16 Tools
    Component:  LibDCPU-pp

    Authors:    James Rhodes

    Description:    Defines the public API for using the preprocessor
            inline in programs.

**/

#include <stdio.h>
#include <bstrlib.h>
#include <simclist.h>
#include "parser.h"
#include "lexer.h"
#include "pp.h"

extern int pp_yyparse(void* scanner);
extern void handle_start(freed_bstring name, FILE* output);

bstring pp_do(freed_bstring path)
{
    FILE* in;
    FILE* out;
    bstring temp;
    yyscan_t scanner;
    freed_bstring friendly;
    int parse_error;

    // Open and set up the temporary output areas.
    temp = bfromcstr(tempnam(".", "pp."));

    if (biseq(path.ref, bfromcstr("-")))
    {
        friendly = bautofree(bfromcstr("-"));
        in = stdin;
    }
    else
    {
        friendly = bautofree(bstrcpy(path.ref));
        in = fopen((const char*)(path.ref->data), "r");
    }

    if (in == NULL)
    {
        fprintf(stderr, "preprocessor: unable to read from input file '%s'.\n", path.ref->data);
        return NULL;
    }

    out = fopen((const char*)(temp->data), "w");

    if (out == NULL)
    {
        if (!biseq(path.ref, bfromcstr("-")))
            fclose(in);

        fprintf(stderr, "preprocessor: unable to write to temporary output file '%s'.\n", temp->data);
        return NULL;
    }

    // Now invoke the preprocessor's lexer and parser.
    pp_yylex_init(&scanner);
    pp_yyset_out(out, scanner);
    pp_yyset_in(in, scanner);
    handle_start(friendly, out);
    parse_error = pp_yyparse(scanner);
    if (parse_error != 0)
    {
        return NULL;
    }
    pp_yylex_destroy(scanner);

    // Now do some cleanup.
    if (!biseq(path.ref, bfromcstr("-")))
        fclose(in);

    fseek(out, 0, SEEK_SET);

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
