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
#include <argtable2.h>
#include <Compiler.h>

extern "C"
{
#include <unistd.h>
#include <version.h>
#include <ppcompat.h>
#include <ppfind.h>
#include <debug.h>
#include <osutil.h>
#include <derr.h>
#include <stdlib.h>
}

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

    if (nerrors != 0 || show_help->count != 0)
    {
        if (nerrors != 0 && show_help->count == 0)
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
    
    // Show version information.
    version_print(bautofree(bfromcstr("Compiler")));

    // Set global path variable.
    osutil_setarg0(bautofree(bfromcstr(argv[0])));

    // Set up error handling. (for PP)
    if (dsethalt())
    {
        // Handle the error.
        dautohandle();
        printd(LEVEL_ERROR, "compiler: error occurred.\n");
        
        arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
        return 1;
    }
    
    // Run the preprocessor.
    ppfind_add_path(bautofree(bfromcstr(".")));
    ppfind_add_path(bautofree(bfromcstr("include")));
    ppfind_add_autopath(bautofree(bfromcstr(input_file->filename[0])));
    for (int i = 0; i < include_dirs->count; ++i)
        ppfind_add_path(bautofree(bfromcstr(include_dirs->filename[i])));
    bstring pp_result_name = pp_do(bautofree(bfromcstr("c")),
            bautofree(bfromcstr(input_file->filename[0])));

    if (pp_result_name == NULL)
    {
        fprintf(stderr, "compiler: invalid result returned from preprocessor.\n");
        pp_cleanup(bautofree(pp_result_name));
        arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
        return 1;
    }

    
    std::ifstream cc_input((const char*)(pp_result_name->data), std::istream::in);
    if (!cc_input.is_open())
    {
        pp_cleanup(bautofree(pp_result_name));
        arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
        return 1;
    }

    // create compiler
    dcpucc::Compiler cc;
    
    // compile code
    cc.compile(cc_input);
    
    // check for errors (in that case exit with error code)
    if (cc.hasErrors())
    {
        cc.printErrors();
        arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
        exit(1);
    }
    
    // check for warnings and print them
    if (cc.hasWarnings())
    {
        cc.printErrors();
    }
   
    // clean up PP
    pp_cleanup(bautofree(pp_result_name));


    /*
     * TODO add assembler support again
    // Assembler type.
    const char* asmtype = "toolchain";

    if (type_assembler->count > 0)
        asmtype = type_assembler->sval[0];
    
    */

    // Initially save to a temporary file.
    std::string temp = std::string(tempnam(".", "cc."));

    // Open the output file.
    if (strcmp(output_file->filename[0], "-") != 0)
    {
        // Write to file.
        std::ofstream output(output_file->filename[0], std::ios::out | std::ios::trunc);
        
        if (output.bad() || output.fail())
        {
            std::cerr << "error: cannot write to output file '" << output_file->filename[0] << "'" << std::endl;
            arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
            return 1;
        }
        output << cc.getAssembler();
        output.close();
    }
    else
    {
        // Set output to cout.
        std::cout << cc.getAssembler();
    }
    
    // free argtable
    arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
    return 0;
}
