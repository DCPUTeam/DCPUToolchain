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
#include <ppcompat.h>
#include <ppfind.h>
#include <version.h>
#include <simclist.h>
#include <ddata.h>
#include <debug.h>
#include <assem.h>
#include <node.h>
#include <derr.h>
#include <aout.h>
#include <parser.h>

extern int yyparse();
extern FILE* yyin, *yyout;
char* fileloc = NULL;

#define EMU_CMD_SIZE 80

void json_demo(struct ast_node_root ast_root);

int main(int argc, char* argv[])
{
    BFILE* out = NULL;
    FILE* img = NULL;
    BFILE* imgb = NULL;
    FILE* sym = NULL;
    int nerrors;
    bstring pp_result_name;
    list_t symbols;
    bstring temp = NULL;
    uint16_t final;
    int i, w;
    const char* warnprefix = "no-";

    // Define arguments.
    struct arg_lit* show_help = arg_litn("h", "help", 0, 2, "Show this help.");
    struct arg_lit* gen_relocatable = arg_lit0("r", "relocatable", "Generate relocatable code.");
    struct arg_lit* gen_intermediate = arg_lit0("i", "intermediate", "(enabled by default; permitted for backwards compatibility)");
    struct arg_lit* no_intermediate = arg_lit0(NULL, "binary", "Directly generate a DCPU-16 binary without a link stage.");
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
    void* argtable[] = { show_help, output_file, symbols_file, warning_policies, gen_relocatable, gen_intermediate, no_intermediate, little_endian_mode, include_dirs, input_file, verbose, quiet, end };

    // Parse arguments.
    nerrors = arg_parse(argc, argv, argtable);

    if (nerrors != 0 || show_help->count != 0)
    {
        if (nerrors != 0 && show_help->count == 0)
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
    
    // Show version information.
    version_print(bautofree(bfromcstr("Assembler")));

    // Set global path variable.
    osutil_setarg0(bautofree(bfromcstr(argv[0])));

    // Set endianness.
    isetmode(little_endian_mode->count == 0 ? IMODE_BIG : IMODE_LITTLE);

    // Set up warning policies.
    dsetwarnpolicy(warning_policies);

    // Set up error handling.
    if (dsethalt())
    {
        // Handle the error.
        dautohandle();
        printd(LEVEL_ERROR, "json-test: error occurred.\n");

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
    pp_result_name = pp_do(bautofree(bfromcstr("asm")),
            bautofree(bfromcstr(input_file->filename[0])));

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
    
    // Fire up the demo.
    json_demo(ast_root);

    arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
    return 0;
}
