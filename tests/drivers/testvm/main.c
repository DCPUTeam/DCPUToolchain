/**

    File:       main.c

    Project:    DCPU-16 Tools
    Component:  Virtual Machine Test Suite

    Authors:    James Rhodes

    Description:    Main entry point.

**/

#include <stdlib.h>
#include <stdio.h>
#include <argtable2.h>
#include <bstring.h>
#include <bfile.h>
#include <version.h>
#include <debug.h>
#include <osutil.h>
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
#endif

int main(int argc, char* argv[])
{
    bstring cmdargs = bfromcstr("");
    int result;
    unsigned int match = 0, unmatch = 0;
    char* file_cpu;
    char* file_sym;
    char* mod_path;
    char* temp;

    // Define arguments.
    struct arg_lit* show_help = arg_lit0("h", "help", "Show this help.");
    struct arg_file* input_file = arg_file1(NULL, NULL, "<file>", "The input assembly file containing unit tests.");
    struct arg_file* path_asm = arg_file1("a", NULL, "<path>", "The path to the assembler.");
    struct arg_file* path_db = arg_file1("d", NULL, "<path>", "The path to the debugger.");
    struct arg_file* path_modules = arg_file1("m", NULL, "<path>", "The path to the modules directory.");
    struct arg_lit* verbose = arg_litn("v", NULL, 0, LEVEL_EVERYTHING - LEVEL_DEFAULT, "Increase verbosity.");
    struct arg_lit* quiet = arg_litn("q", NULL,  0, LEVEL_DEFAULT - LEVEL_SILENT, "Decrease verbosity.");
    struct arg_end* end = arg_end(20);
    void* argtable[] = { show_help, input_file, path_asm, path_db, path_modules, verbose, quiet, end };

    // Parse arguments.
    int nerrors = arg_parse(argc, argv, argtable);

    version_print(bautofree(bfromcstr("Virtual Machine Unit Test Driver")));
    if (nerrors != 0 || show_help->count != 0)
    {
        printd(LEVEL_DEFAULT, "syntax:\n    testvm");
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

    // Get temporary names for each intermediate file.
    file_cpu = tempnam(NULL, "vtcpu");
    file_sym = tempnam(NULL, "vtsym");

    // We need to get the existing TOOLCHAIN_MODULES
    // environment variable so we can set it back later
    // on.
    mod_path = getenv("TOOLCHAIN_MODULES");
#ifdef WIN32
    SetEnvironmentVariable("TOOLCHAIN_MODULES", path_modules->filename[0]);
#else
    setenv("TOOLCHAIN_MODULES", path_modules->filename[0], 1);
#endif

    // Generate the argument list for the assembler.
    bassigncstr(cmdargs, path_asm->filename[0]);
    binsertch(cmdargs, 0, 1, '"');
    bconchar(cmdargs, '"');
    bconchar(cmdargs, ' ');

    // Binary mode.
    bcatcstr(cmdargs, "--binary ");
    
    // Symbols file.
    bcatcstr(cmdargs, "-s \"");
    bcatcstr(cmdargs, file_sym);
    bcatcstr(cmdargs, "\" ");

    // Output file.
    bcatcstr(cmdargs, "-o \"");
    bcatcstr(cmdargs, file_cpu);
    bcatcstr(cmdargs, "\" ");

    // Input file.
    bcatcstr(cmdargs, "\"");
    bcatcstr(cmdargs, input_file->filename[0]);
    bcatcstr(cmdargs, "\" ");

    // Windows needs the whole command wrapped in quotes and slashes to be correct.
    // See http://stackoverflow.com/questions/2642551/windows-c-system-call-with-spaces-in-command.
#ifdef _WIN32
    binsertch(cmdargs, 0, 1, '"');
    bconchar(cmdargs, '"');
#endif

    // Now run the assembler!
    result = system(cmdargs->data);
    if (result != 0)
    {
        // Assembler returned error exit code.
        printd(LEVEL_ERROR, "command run was: %s\n", cmdargs->data);
        printd(LEVEL_ERROR, "error: unable to assemble unit test (assembler returned non-zero exit code '%i').\n", result >> 8);
        return 1;
    }

    // We now need to set up the command to
    // run the debugger to invoke the unit tests.
    bassigncstr(cmdargs, path_db->filename[0]);
    binsertch(cmdargs, 0, 1, '"');
    bconchar(cmdargs, '"');
    bconchar(cmdargs, ' ');

    // Do not attach devices during testing.
    bcatcstr(cmdargs, "-n ");

    // Run the 'test' command automatically.
    bcatcstr(cmdargs, "-c test ");

    // Symbols file.
    bcatcstr(cmdargs, "-s \"");
    bcatcstr(cmdargs, file_sym);
    bcatcstr(cmdargs, "\" ");

    // Input file.
    bcatcstr(cmdargs, "\"");
    bcatcstr(cmdargs, file_cpu);
    bcatcstr(cmdargs, "\" ");

    // Windows needs the whole command wrapped in quotes and slashes to be correct.
    // See http://stackoverflow.com/questions/2642551/windows-c-system-call-with-spaces-in-command.
#ifdef _WIN32
    binsertch(cmdargs, 0, 1, '"');
    bconchar(cmdargs, '"');
#endif

    // Now run the debugger!
    result = system(cmdargs->data);
    if (result != 0)
    {
        // Unit test failed!
        unlink(file_cpu);
        unlink(file_sym);

        // Restore TOOLCHAIN_MODULES.
        if (mod_path == NULL)
        {
#ifdef WIN32
            SetEnvironmentVariable("TOOLCHAIN_MODULES", NULL);
#else
            unsetenv("TOOLCHAIN_MODULES");
#endif
        }
        else
        {
            bassigncstr(cmdargs, mod_path);
#ifdef WIN32
            SetEnvironmentVariable("TOOLCHAIN_MODULES", temp = bstr2cstr(cmdargs, '0'));
#else
            setenv("TOOLCHAIN_MODULES", temp = bstr2cstr(cmdargs, '0'), 1);
#endif
            bcstrfree(temp);
        }

        // Show error message.
#ifdef _WIN32
        printd(LEVEL_ERROR, "error: unit test failed with exit code %i (see above output).\n", result);
#else
        // On UNIX, we have to bitshift to get the actual program error code.
        printd(LEVEL_ERROR, "error: unit test failed with exit code %i (see above output).\n", result >> 8);
#endif
        return 1;
    }

    // Delete temporary files.
    unlink(file_cpu);
    unlink(file_sym);

    // Restore TOOLCHAIN_MODULES.
    if (mod_path == NULL)
    {
#ifdef WIN32
        SetEnvironmentVariable("TOOLCHAIN_MODULES", NULL);
#else
        unsetenv("TOOLCHAIN_MODULES");
#endif
    }
    else
    {
        bassigncstr(cmdargs, mod_path);
#ifdef WIN32
        SetEnvironmentVariable("TOOLCHAIN_MODULES", temp = bstr2cstr(cmdargs, '0'));
#else
        setenv("TOOLCHAIN_MODULES", temp = bstr2cstr(cmdargs, '0'), 1);
#endif
        bcstrfree(temp);
    }

    return 0;
}
