/**

    File:       main.c

    Project:    DCPU-16 Tools
    Component:  Preprocessor

    Authors:    James Rhodes

    Description:    Main entry point.

**/

#include <argtable2.h>
#include <dcpu.h>
#include <ppimpl.h>
#include <derr.h>
#include <osutil.h>
#include <ppfind.h>
#include <debug.h>
#include <stdlib.h>

FILE* file_input = NULL;
char next = '\0';

bool has_input(void* userdata)
{
    return !feof(file_input);
}

char input(void* userdata)
{
    next = (char)getc(file_input);
    return next;
}

void output(void* userdata, char o)
{
    putc((int)o, stdout);
}

int main(int argc, char* argv[])
{
    int nerrors, w;
    bstring temp = NULL;
    struct errinfo* errval;
    const char* prepend = "error: ";
    const char* warnprefix = "no-";
    int msglen;
    char* msg;
    
    // Define arguments.
    struct arg_lit* show_help = arg_lit0("h", "help", "Show this help.");
    struct arg_str* input_lang = arg_str1(NULL, NULL, "<lang>", "The input language.");
    struct arg_file* input_file = arg_file1(NULL, NULL, "<file>", "The input preprocessor file.");
    struct arg_lit* verbose = arg_litn("v", NULL, 0, LEVEL_EVERYTHING - LEVEL_DEFAULT, "Increase verbosity.");
    struct arg_lit* quiet = arg_litn("q", NULL,  0, LEVEL_DEFAULT - LEVEL_SILENT, "Decrease verbosity.");
    struct arg_end* end = arg_end(20);
    void* argtable[] = { show_help, verbose, quiet, input_lang, input_file, end };

    // Parse arguments.
    nerrors = arg_parse(argc, argv, argtable);

    if (nerrors != 0 || show_help->count != 0)
    {
        if (nerrors != 0 && show_help->count == 0)
            arg_print_errors(stdout, end, "preprocessor");

        printd(LEVEL_DEFAULT, "syntax:\n    dtpp");
        arg_print_syntax(stderr, argtable, "\n");
        printd(LEVEL_DEFAULT, "options:\n");
        arg_print_glossary(stderr, argtable, "    %-25s %s\n");
        if (show_help->count == 2)
        {
            printd(LEVEL_DEFAULT, "defined warnings:\n");
            for (w = 0; w < _WARN_COUNT; w++)
                printd(LEVEL_DEFAULT, "      %s\n", dwarnpolicy[w].name);
        }
        arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
        return 1;
    }

    // Set verbosity level.
    debug_setlevel(LEVEL_DEFAULT + verbose->count - quiet->count);

    // Set global path variable.
    osutil_setarg0(bautofree(bfromcstr(argv[0])));

    // Set up error handling.
    if (dsethalt())
    {
        errval = derrinfo();

        // FIXME: Use bstrings here.
        msglen = strlen(derrstr[errval->errid]) + strlen(prepend) + 1;
        msg = malloc(msglen);
        memset(msg, '\0', msglen);
        strcat(msg, prepend);
        strcat(msg, derrstr[errval->errid]);
        printd(LEVEL_ERROR, msg, errval->errdata);

        // Handle the error.
        printd(LEVEL_ERROR, "libdcpu-pp test suite: error occurred.\n");
        
        arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
        return 1;
    }
    
    // Open the file if needed.
    if (strcmp(input_file->filename[0], "-") == 0)
        file_input = stdin;
    else
        file_input = fopen(input_file->filename[0], "r");
    
    ppimpl(bautofree(bfromcstr("<stdin>")), 0, bautofree(bfromcstr(*input_lang->sval)), has_input, input, output, NULL);
    
    if (strcmp(input_file->filename[0], "-") != 0)
        fclose(file_input);
    
    return 0;
}
