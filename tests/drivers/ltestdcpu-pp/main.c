///
/// @addtogroup Tests
/// @{
///
/// @file
/// @brief Tests the preprocessor and internal line tracking.
///

#include <argtable2.h>
#include <debug.h>
#include <bfile.h>
#include <stdlib.h>
#include <bstring.h>
#include <ppcompat.h>
#include <osutil.h>
#include <iio.h>
#include <derr.h>
#include <ppfind.h>
#include <dcpu.h>
#include "parser.h"

extern int yyparse();
extern FILE* yyin;
extern FILE* yyout;
int test_success;

int main(int argc, char* argv[])
{
    int nerrors, w;
    bstring temp = NULL;
    struct errinfo* errval;
    const char* prepend = "error: ";
    const char* warnprefix = "no-";
    int msglen;
    char* msg;
    bool expect_failure;
    
    // Define arguments.
    struct arg_lit* show_help = arg_lit0("h", "help", "Show this help.");
    struct arg_lit* expect_failure_lit = arg_lit0("f", "fail", "Expect failure during preprocessing.");
    struct arg_str* input_lang = arg_str1(NULL, NULL, "<lang>", "The input language.");
    struct arg_file* input_file = arg_file1(NULL, NULL, "<file>", "The input preprocessor file.");
    struct arg_lit* verbose = arg_litn("v", NULL, 0, LEVEL_EVERYTHING - LEVEL_DEFAULT, "Increase verbosity.");
    struct arg_lit* quiet = arg_litn("q", NULL,  0, LEVEL_DEFAULT - LEVEL_SILENT, "Decrease verbosity.");
    struct arg_end* end = arg_end(20);
    void* argtable[] = { show_help, verbose, quiet, expect_failure_lit, input_lang, input_file, end };
    
    // Parse arguments.
    nerrors = arg_parse(argc, argv, argtable);

    if (nerrors != 0 || show_help->count != 0)
    {
        if (nerrors != 0)
            arg_print_errors(stdout, end, "libdcpu-pp test suite");

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
    
    // Set failure expectation variable.
    expect_failure = (expect_failure_lit->count > 0);

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

        if (temp != NULL)
            pp_cleanup(bautofree(temp));

        arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
        if (expect_failure)
            return 0;
        else
            return 1;
    }
    
    // Perform preprocessing.
    ppfind_add_autopath(bautofree(bfromcstr(input_file->filename[0])));
    temp = pp_do(
        bautofree(bfromcstr(input_lang->sval[0])),
        bautofree(bfromcstr(input_file->filename[0]))
    );
    
    // Perform parsing.
    test_success = true;
    yyin = fopen(temp->data, "r");
    yyout = NULL;
    yyparse();
    fclose(yyin);
    
    // Cleanup.
    pp_cleanup(bautofree(temp));
    
    if (test_success && expect_failure)
        return 1;
    else if (test_success && !expect_failure)
        return 0;
    else if (!test_success && expect_failure)
        return 0;
    else
        return 1;
}

///
/// @}
///