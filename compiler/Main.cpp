/**

	File:			main.cpp

	Project:		DCPU-16 Tools
	Component:		Compiler

	Authors:		James Rhodes

	Description:	Main entry point.

**/

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <algorithm>
#include "node.h"
#include "asmgen.h"
#include "argtable2.h"
extern "C"
{
	#include "pp.h"
}
extern int yyparse();
extern FILE *yyin, *yyout;
extern NDeclarations* program;

// Utility directory name function (TODO: Move this into it's own file).
template<typename string_t>
string_t dirname(string_t source)
{
    if (source.size() <= 1) //Make sure it's possible to check the last character.
    {
        return source;
    }
    if (*(source.rbegin() + 1) == '/') //Remove trailing slash if it exists.
    {
        source.pop_back();
    }
    source.erase(std::find(source.rbegin(), source.rend(), '/').base(), source.end());
    return source;
}

int main(int argc, char* argv[])
{
	// Define arguments.
	struct arg_lit* show_help = arg_lit0("h", "help", "Show this help.");
	struct arg_str* type_assembler = arg_str0("t", NULL, "<type>", "The type of assembler to output for.");
	struct arg_lit* invoke_assembler = arg_lit0("a", NULL, "Invoke the assembler on the output automatically.");
	struct arg_lit* invoke_emulator = arg_lit0("e", NULL, "Invoke the emulator on the assembled output automatically.");
	struct arg_file* input_file = arg_file1(NULL, NULL, "<file>", "The input file.");
	struct arg_file* output_file = arg_file1("o", "output", "<file>", "The output file.");
	struct arg_end *end = arg_end(20);
	void *argtable[] = { output_file, show_help, type_assembler, invoke_assembler, invoke_emulator, input_file, end };

	// Parse arguments.
	int nerrors = arg_parse(argc,argv,argtable);
	if (nerrors != 0 || show_help->count != 0)
	{
		if (show_help->count != 0)
			arg_print_errors(stdout, end, "compiler");
		printf("\syntax:\n    compiler");
		arg_print_syntax(stdout, argtable, "\n");
		printf("\options:\n");
		arg_print_glossary(stdout, argtable, "    %-25s %s\n");
		exit(1);
	}

	// Parse C.
	pp_add_search_path(".");
	pp_add_search_path(dirname<std::string>(input_file->filename[0]).c_str());
	yyin = pp_do(input_file->filename[0]);
	if (yyin == NULL)
	{
		pp_cleanup();
		return 1;
	}
	yyparse();
	fclose(yyin);
	pp_cleanup();
	if (program == NULL)
	{
		std::cout << "An error occurred while compiling." << std::endl;
		return 1;
	}

	// Assembler type.
	const char* asmtype = "dcpu16tools";
	if (type_assembler->count > 0)
		asmtype = type_assembler->sval[0];

	// Spacing.
	std::cout << std::endl;

	// Generate assembly using the AST.
	try
	{
		std::ofstream output(output_file->filename[0], std::ios::out | std::ios::trunc);
		AsmGenerator generator(asmtype);
		AsmBlock* block = program->compile(generator);
		output << generator.m_Preassembly << std::endl;
		output << *block << std::endl;
		output << generator.m_Postassembly << std::endl;
		output.close();
		delete block;
	}
	catch (CompilerException* ex)
	{
		std::string msg = ex->getMessage();
		std::cout << "An error occurred while compiling." << std::endl;
		std::cout << msg << std::endl;
		return 1;
	}

	// Execute emulator if desired.
	if (invoke_assembler->count > 0)
	{
		std::string str = "assembler ";
		if (invoke_emulator->count > 0)
			str += "-e ";
		str += "-o \"";
		str += output_file->filename[0];
		str += ".dcpu16\" \"";
		str += output_file->filename[0];
		str += "\"";
		std::cout << "Executing: " << str << std::endl;
		int asmres = system(str.c_str());
		if (asmres == 0)
			std::cout << "Assembler exited successfully." << std::endl;
		else
			std::cout << "Assembler exited with code " << asmres << "." << std::endl;
	}
	else if (invoke_emulator->count > 0)
		std::cout << "Can't run the emulator without assembling (-a) first.  Ignoring -e option." << std::endl;
	
	return 0;
}