/**

	File:		main.cpp

	Project:	DCPU-16 Tools
	Component:	Compiler

	Authors:	James Rhodes

	Description:	Main entry point.

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
#include <version.h>
#include <pp.h>
#include <ppfind.h>
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
	struct arg_end* end = arg_end(20);
	void* argtable[] = { output_file, show_help, type_assembler, input_file, end };

	// Parse arguments.
	int nerrors = arg_parse(argc, argv, argtable);

	version_print(bautofree(bfromcstr("Compiler")));
	if (nerrors != 0 || show_help->count != 0)
	{
		if (show_help->count != 0)
			arg_print_errors(stderr, end, "compiler");

		fprintf(stderr, "syntax:\n    dtcc");
		arg_print_syntax(stderr, argtable, "\n");
		fprintf(stderr, "options:\n");
		arg_print_glossary(stderr, argtable, "	  %-25s %s\n");
		arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
		return 1;
	}

	// Run the preprocessor.
	ppfind_add_path(bautofree(bfromcstr(".")));
	ppfind_add_path(bautofree(bfromcstr("include")));
	ppfind_add_autopath(bautofree(bfromcstr(input_file->filename[0])));
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
	const char* asmtype = "dcpu16toolchain";

	if (type_assembler->count > 0)
		asmtype = type_assembler->sval[0];

	// Spacing.
	std::cerr << std::endl;

	// Generate assembly using the AST.
	try
	{
		AsmGenerator generator(asmtype);
		AsmBlock* block = program->compile(generator);

		if (strcmp(output_file->filename[0], "-") == 0)
		{
			std::cout << generator.m_Preassembly << std::endl;
			std::cout << *block << std::endl;
			std::cout << generator.m_Postassembly << std::endl;
		}
		else
		{
			std::ofstream output(output_file->filename[0], std::ios::out | std::ios::trunc);
			output << generator.m_Preassembly << std::endl;
			output << *block << std::endl;
			output << generator.m_Postassembly << std::endl;
			output.close();
		}

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

	arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
	return 0;
}
