/**

    File:       main.c

    Project:    DCPU-16 Tools
    Component:  Assembler

    Authors:    James Rhodes

    Description:    Main entry point.

**/

#ifndef WIN32
#include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <bstring.h>
#include <argtable2.h>
#include <bfile.h>
#include <iio.h>
#include <osutil.h>
#include <pp.h>
#include <ppfind.h>
#include <version.h>
#include <simclist.h>
#include <ddata.h>
#include <debug.h>
#include <assem.h>
#include <node.h>
#include <derr.h>
#include <aout.h>

extern int yyparse();
extern FILE* yyin, *yyout;
char* fileloc = NULL;

#define EMU_CMD_SIZE 80

int main(int argc, char* argv[])
{
    BFILE* out = NULL;
    FILE* img = NULL;
    BFILE* imgb = NULL;
    FILE* sym = NULL;
    int nerrors;
    bstring pp_result_name;
    struct errinfo* errval;
    list_t symbols;
    bstring temp = NULL;
    uint16_t final;
    int i, w;
    const char* prepend = "error: ";
    const char* warnprefix = "no-";
    int msglen;
    char* msg;

    // Define arguments.
    struct arg_lit* show_help = arg_litn("h", "help", 0, 2, "Show this help.");
    struct arg_lit* gen_relocatable = arg_lit0("r", "relocatable", "Generate relocatable code.");
    struct arg_lit* gen_intermediate = arg_lit0("i", "intermediate", "Generate intermediate code for use with the linker.");
    // 20 is maxcount for include directories, this has to be set to some constant number.
    struct arg_file* include_dirs = arg_filen("I", NULL, "<directory>", 0, 20, "Adds the directory <dir> to the directories to be searched for header files.");
    struct arg_file* input_file = arg_file1(NULL, NULL, "<file>", "The input file (or - to read from standard input).");
    struct arg_file* output_file = arg_file1("o", "output", "<file>", "The output file (or - to send to standard output).");
    struct arg_file* symbols_file = arg_file0("s", "debug-symbols", "<file>", "The debugging symbol output file.");
    struct arg_str* warning_policies = arg_strn("W", NULL, "policy", 0, _WARN_COUNT * 2 + 10, "Modify warning policies.");
    struct arg_lit* little_endian_mode = arg_lit0(NULL, "little-endian", "Use little endian serialization (for compatibility with older versions).");
    struct arg_lit* verbose = arg_litn("v", NULL, 0, LEVEL_EVERYTHING - LEVEL_DEFAULT, "Increase verbosity.");
    struct arg_lit* quiet = arg_litn("q", NULL,  0, LEVEL_DEFAULT - LEVEL_SILENT, "Decrease verbosity.");
    struct arg_end* end = arg_end(20);
    void* argtable[] = { show_help, output_file, symbols_file, warning_policies, gen_relocatable, gen_intermediate, little_endian_mode, include_dirs, input_file, verbose, quiet, end };

    // Parse arguments.
    nerrors = arg_parse(argc, argv, argtable);

    version_print(bautofree(bfromcstr("Assembler")));
    if (nerrors != 0 || show_help->count != 0)
    {
        if (nerrors != 0)
            arg_print_errors(stdout, end, "assembler");

        printd(LEVEL_DEFAULT, "syntax:\n    dtasm");
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

    // Set endianness.
    isetmode(little_endian_mode->count == 0 ? IMODE_BIG : IMODE_LITTLE);

    // Set up warning policies.
    dsetwarnpolicy(warning_policies);

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
        printd(LEVEL_ERROR, "assembler: error occurred.\n");

        if (img != NULL)
            fclose(img);
        if (temp != NULL)
            unlink(temp->data);

        arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
        return 1;
    }

    // Run the preprocessor.
    ppfind_add_autopath(bautofree(bfromcstr(input_file->filename[0])));
    for (i = 0; i < include_dirs->count; ++i)
        ppfind_add_path(bautofree(bfromcstr(include_dirs->filename[i])));
    pp_result_name = pp_do(bautofree(bfromcstr(input_file->filename[0])));

    if (pp_result_name == NULL)
    {
        printd(LEVEL_ERROR, "assembler: invalid result returned from preprocessor.\n");
        pp_cleanup(bautofree(pp_result_name));
        arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
        return 1;
    }

    yyout = stderr;
    yyin = fopen((const char*)(pp_result_name->data), "r");

    if (yyin == NULL)
    {
        pp_cleanup(bautofree(pp_result_name));
        arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
        return 1;
    }

    // Parse assembly.
    yyparse();

    if (yyin != stdin)
        fclose(yyin);

    pp_cleanup(bautofree(pp_result_name));

    if (&ast_root == NULL || ast_root.values == NULL)
    {
        printd(LEVEL_ERROR, "assembler: syntax error, see above.\n");
        arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
        return 1;
    }

    // Initialize storage for debugging symbols.
    list_init(&symbols);

    // Process AST.
    process_root(&ast_root, &symbols);

    // Initially save to a temporary file.
    temp = bfromcstr(tempnam(".", "asm."));
    img = fopen(temp->data, "wb");
    if (img == NULL)
    {
        printd(LEVEL_ERROR, "assembler: temporary file not writable.\n");
        arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
        return 1;
    }

    // Write content.
    final = aout_write(img, (gen_relocatable->count > 0), (gen_intermediate->count > 0));
    fclose(img);
    printd(LEVEL_VERBOSE, "assembler: completed successfully.\n");

    // Save debugging symbols to specified file if provided.
    if (symbols_file->count > 0)
    {
        // Set the address of any debugging symbols that don't have an address
        // yet to the address at the end of the file.
        list_iterator_start(&symbols);
        while (list_iterator_hasnext(&symbols))
            dbgfmt_finalize_symbol(list_iterator_next(&symbols), final);
        list_iterator_stop(&symbols);

        // Check to ensure that the user isn't trying to write symbol files to stdout.
        if (strcmp(symbols_file->filename[0], "-") == 0)
        {
            printd(LEVEL_ERROR, "assembler: debugging symbols can not be written to stdout.\n");
            arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
            return 1;
        }

        // Write symbols.
        dbgfmt_write(bfromcstr(symbols_file->filename[0]), &symbols);
        printd(LEVEL_VERBOSE, "assembler: wrote debugging symbols.\n");
    }

    // Re-open the temporary file for reading.
    imgb = bfopen(temp->data, "rb");
    if (imgb == NULL)
    {
        printd(LEVEL_ERROR, "assembler: temporary file not readable.\n");
        arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
        return 1;
    }

    // Open the output file.
    if (strcmp(output_file->filename[0], "-") != 0)
    {
        // Write to file.
        out = bfopen(output_file->filename[0], "wb");

        if (imgb == NULL)
        {
            printd(LEVEL_ERROR, "assembler: output file not writable.\n");
            arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
            return 1;
        }
    }
    else
    {
        // Windows needs stdout in binary mode.
        osutil_makebinary(stdout);

        // Set img to stdout.
        out = bfwrap(stdout, "w");
    }

    // Copy data.
    while (!bfeof(imgb))
        bfputc(bfgetc(imgb), out);

    // Close files and delete temporary.
    bfclose(imgb);
    bfclose(out);
    unlink(temp->data);

    arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
    return 0;
}
