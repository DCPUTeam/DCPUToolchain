/**

    File:       main.cpp

    Project:    DCPU-16 Tools
    Component:  Compiler

    Authors:    James Rhodes

    Description:    Main entry point.

**/

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <nodes/NDeclarations.h>
#include <StackMap.h>
#include <AsmGenerator.h>
#include <CompilerException.h>
#include <argtable2.h>

extern "C"
{
#include <unistd.h>
#include <version.h>
#include <pp.h>
#include <ppfind.h>
#include <debug.h>
#include <osutil.h>
}

extern int yyparse();
extern FILE* yyin, *yyout;
extern NDeclarations* program;

int main(int argc, char* argv[])
{
    // Define arguments.
    struct arg_lit* show_help = arg_lit0("h", "help", "Show this help.");
    struct arg_str* type_assembler = arg_str0("t", NULL, "<type>", "The type of assembler to output for.");
    struct arg_file* input_file = arg_file1(NULL, NULL, "<file>", "The input file (or - to read from standard input).");
    struct arg_file* output_file = arg_file1("o", "output", "<file>", "The output file (or - to send to standard output).");
    // 20 is maxcount for include directories, this has to be set to some constant number.
    struct arg_file* include_dirs = arg_filen("I", NULL, "<directory>", 0, 20, "Adds the directory <dir> to the directories to be searched for header files.");
    struct arg_lit* verbose = arg_litn("v", NULL, 0, LEVEL_EVERYTHING - LEVEL_DEFAULT, "Increase verbosity.");
    struct arg_lit* quiet = arg_litn("q", NULL,  0, LEVEL_DEFAULT - LEVEL_SILENT, "Decrease verbosity.");
    struct arg_end* end = arg_end(20);
    void* argtable[] = { output_file, show_help, type_assembler, include_dirs, input_file, verbose, quiet, end };

    // Parse arguments.
    int nerrors = arg_parse(argc, argv, argtable);

    version_print(bautofree(bfromcstr("Compiler")));
    if (nerrors != 0 || show_help->count != 0)
    {
        if (nerrors != 0)
            arg_print_errors(stderr, end, "compiler");

        fprintf(stderr, "syntax:\n    dtcc");
        arg_print_syntax(stderr, argtable, "\n");
        fprintf(stderr, "options:\n");
        arg_print_glossary(stderr, argtable, "    %-25s %s\n");
        arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
        return 1;
    }

    // Set verbosity level.
    debug_setlevel(LEVEL_DEFAULT + verbose->count - quiet->count);

    // Set global path variable.
    osutil_setarg0(bautofree(bfromcstr(argv[0])));

    // Run the preprocessor.
    ppfind_add_path(bautofree(bfromcstr(".")));
    ppfind_add_path(bautofree(bfromcstr("include")));
    ppfind_add_autopath(bautofree(bfromcstr(input_file->filename[0])));
    for (int i = 0; i < include_dirs->count; ++i)
        ppfind_add_path(bautofree(bfromcstr(include_dirs->filename[i])));
    bstring pp_result_name = pp_do(bautofree(bfromcstr(input_file->filename[0])));

    if (pp_result_name == NULL)
    {
        fprintf(stderr, "compiler: invalid result returned from preprocessor.\n");
        pp_cleanup(bautofree(pp_result_name));
        arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
        return 1;
    }

    // Parse C.
    yyout = stderr;
    yyin = fopen((const char*)(pp_result_name->data), "r");

    if (yyin == NULL)
    {
        pp_cleanup(bautofree(pp_result_name));
        arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
        return 1;
    }

    yyparse();

    if (yyin != stdin)
        fclose(yyin);

    pp_cleanup(bautofree(pp_result_name));

    if (program == NULL)
    {
        std::cerr << "An error occurred while compiling." << std::endl;
        arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
        return 1;
    }

    // Assembler type.
    const char* asmtype = "toolchain";

    if (type_assembler->count > 0)
        asmtype = type_assembler->sval[0];

    // Initially save to a temporary file.
    std::string temp = std::string(tempnam(".", "cc."));

    // Generate assembly using the AST.
    try
    {
        AsmGenerator generator(asmtype);
        AsmBlock* block = program->compile(generator);

        std::ofstream output(temp.c_str(), std::ios::out | std::ios::trunc);
        if (output.bad() || output.fail())
        {
            printd(LEVEL_ERROR, "compiler: temporary file not writable.\n");
            arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
            return 1;
        }
        output << *block << std::endl;
        output.close();

        delete block;
    }
    catch (CompilerException* ex)
    {
        std::string msg = ex->getMessage();
        std::cerr << "An error occurred while compiling." << std::endl;
        std::cerr << msg << std::endl;
        arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
        return 1;
    }

    // Re-open the temporary file for reading.
    std::ifstream input(temp.c_str(), std::ios::in);
    if (input.bad() || input.fail())
    {
        printd(LEVEL_ERROR, "compiler: temporary file not readable.\n");
        arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
        return 1;
    }

    // Open the output file.
    std::ostream* output;
    if (strcmp(output_file->filename[0], "-") != 0)
    {
        // Write to file.
        output = new std::ofstream(output_file->filename[0], std::ios::out | std::ios::trunc);

        if (output->bad() || output->fail())
        {
            printd(LEVEL_ERROR, "compiler: output file not readable.\n");
            arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
            return 1;
        }
    }
    else
    {
        // Set output to cout.
        output = &std::cout;
    }

    // Copy data.
    std::copy(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>(), std::ostreambuf_iterator<char>(*output));

    // Close files and delete temporary.
    if (strcmp(output_file->filename[0], "-") != 0)
    {
        ((std::ofstream*)output)->close();
        delete output;
    }
    input.close();
    unlink(temp.c_str());

    arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
    return 0;
}
