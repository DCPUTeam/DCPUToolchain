/**

    File:       main.c

    Project:    DCPU-16 Tools
    Component:  Linker Test Suite

    Authors:    James Rhodes

    Description:    Main entry point.

**/

#include <stdlib.h>
#include <stdio.h>
#include <argtable2.h>
#include <bstring.h>
#include <version.h>
#include <debug.h>
#include <osutil.h>
#ifndef WIN32
#include <unistd.h>
#endif

int main(int argc, char* argv[])
{
    bstring ldargs = bfromcstr("");
    int i, result;
    unsigned int match = 0, unmatch = 0;
    char ca, ce;
    FILE* expect;
    FILE* actual;

    // Define arguments.
    struct arg_lit* show_help = arg_lit0("h", "help", "Show this help.");
    struct arg_str* target_arg = arg_str0("l", "link-as", "target", "Link as the specified object, can be 'image' or 'static'.");
    struct arg_file* input_files = arg_filen(NULL, NULL, "<file>", 1, 100, "The input object files.");
    struct arg_file* expect_file = arg_file0("e", "expect", "<file>", "The output file that contains expected output.");
    struct arg_file* actual_file = arg_file1("a", "actual", "<file>", "The output file where actual output will be placed.");
    struct arg_lit* fail_opt = arg_lit0("f", "fail", "The linker is expected to fail and the actual output file should not exist on completion.");
    struct arg_file* path = arg_file1("p", NULL, "<path>", "The path to the linker.");
    struct arg_lit* verbose = arg_litn("v", NULL, 0, LEVEL_EVERYTHING - LEVEL_DEFAULT, "Increase verbosity.");
    struct arg_lit* quiet = arg_litn("q", NULL,  0, LEVEL_DEFAULT - LEVEL_SILENT, "Decrease verbosity.");
    struct arg_end* end = arg_end(20);
    void* argtable[] = { show_help, target_arg, input_files, expect_file, actual_file, fail_opt, path, verbose, quiet, end };

    // Parse arguments.
    int nerrors = arg_parse(argc, argv, argtable);

    version_print(bautofree(bfromcstr("Linker Test Driver")));
    if (nerrors != 0 || show_help->count != 0 || (fail_opt->count == 0 && (expect_file->count == 0 || actual_file->count == 0)))
    {
        if (show_help->count != 0 && fail_opt->count == 0 && (expect_file->count == 0 || actual_file->count == 0))
            printd(LEVEL_DEFAULT, "you must provide either -f or -e and -a.\n");
        if (show_help->count != 0)
            arg_print_errors(stderr, end, "testld");

        printd(LEVEL_DEFAULT, "syntax:\n    testld");
        arg_print_syntax(stderr, argtable, "\n");
        printd(LEVEL_DEFAULT, "options:\n");
        arg_print_glossary(stderr, argtable, "    %-25s %s\n");
        arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
        return 1;
    }

    // Set verbosity level.
    debug_setlevel(LEVEL_DEFAULT + verbose->count - quiet->count);

    // Set global path variable.
    osutil_setarg0(bautofree(bfromcstr(argv[0])));

    // Generate the argument list for the linker.
    ldargs = bfromcstr(path->filename[0]);
    binsertch(ldargs, 0, 1, '"');
    bconchar(ldargs, '"');
    bconchar(ldargs, ' ');

    // Verbosity options.
    if (verbose->count > 0)
    {
        bconchar(ldargs, '-');
        for (i = 0; i < verbose->count; i++) bconchar(ldargs, 'v');
        bconchar(ldargs, ' ');
    }
    if (quiet->count > 0)
    {
        bconchar(ldargs, '-');
        for (i = 0; i < quiet->count; i++) bconchar(ldargs, 'q');
        bconchar(ldargs, ' ');
    }

    // Unlink the actual file so that if we are expecting
    // failure, we won't return incorrectly.
    unlink(actual_file->filename[0]);

    // Do not attempt to link against a kernel in the test suite.
    bcatcstr(ldargs, "-k none ");
    
    // Output option.
    bcatcstr(ldargs, "-o \"");
    bcatcstr(ldargs, actual_file->filename[0]);
    bcatcstr(ldargs, "\" ");

    // Input files.
    for (i = 0; i < input_files->count; i++)
    {
        bcatcstr(ldargs, "\"");
        bcatcstr(ldargs, input_files->filename[i]);
        bcatcstr(ldargs, "\" ");
    }

    // Windows needs the whole command wrapped in quotes and slashes to be correct.
    // See http://stackoverflow.com/questions/2642551/windows-c-system-call-with-spaces-in-command.
#ifdef _WIN32
    binsertch(ldargs, 0, 1, '"');
    bconchar(ldargs, '"');
#endif

    // Now run the linker!
    printd(LEVEL_DEFAULT, "%s\n", ldargs->data);
    result = system(ldargs->data);
    if (result != 0 && fail_opt->count == 0)
    {
        // Linker returned error exit code.
        printd(LEVEL_ERROR, "error: expected success but linker returned non-zero exit code (%i).", result);
        return 1;
    }
    else if (result == 0 && fail_opt->count >= 1)
    {
        // Linker returned zero when failure was expected.
        printd(LEVEL_ERROR, "error: expected failure but linker returned zero exit code.");
        return 1;
    }
    else if (result != 0 && fail_opt->count >= 1)
    {
        // Linker failed and we expected it to.  Return success only
        // if the output file does not exist.
        actual = fopen(actual_file->filename[0], "rb");
        if (actual != NULL)
        {
            printd(LEVEL_ERROR, "error: expected failure but actual output file exists.");
            fclose(actual);
            return 1;
        }
        return 0;
    }

    // Open expect data.
    expect = fopen(expect_file->filename[0], "rb");
    if (expect == NULL)
    {
        // The expect file was not provided.
        printd(LEVEL_ERROR, "error: path to expect file does not exist.");
        return 1;
    }

    // Open actual data.
    actual = fopen(actual_file->filename[0], "rb");
    if (actual == NULL)
    {
        // The expect file was not provided.
        fclose(expect);
        printd(LEVEL_ERROR, "error: expected data but actual output file does not exist after running linker.");
        return 1;
    }

    // Now compare raw bytes.
    while (!feof(actual) && !feof(expect))
    {
        ca = fgetc(actual);
        ce = fgetc(expect);
        if (ca == ce)
            match++;
        else
        {
            printd(LEVEL_WARNING, "warning: byte at 0x%04X is different (got 0x%02X, expected 0x%02X)!\n", ftell(actual), ca, ce);
            unmatch++;
        }
    }
    if (unmatch > 0)
    {
        printd(LEVEL_ERROR, "error: actual output differs from expected output in content (%f%%, %i bytes different).\n", 100.f / (unmatch + match) * unmatch, unmatch);
        if (ftell(actual) != ftell(expect))
            printd(LEVEL_ERROR, "error: actual output differs from expected output in length (%i bytes larger).\n", ftell(expect) - ftell(actual));
        fclose(actual);
        fclose(expect);
        return 1;
    }

    // Compare positions (to ensure same length).
    if (ftell(actual) != ftell(expect))
    {
        printd(LEVEL_ERROR, "error: actual output differs from expected output in length (%i bytes difference).\n", ftell(expect) - ftell(actual));
        fclose(actual);
        fclose(expect);
        return 1;
    }

    // Close files and delete actual because we have
    // succeeded.
    fclose(actual);
    fclose(expect);
    unlink(actual_file->filename[0]);

    return 0;
}
