//
// DCPU Assembler by James Rhodes
//
// Main entry point.
//

#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <argtable2.h>
#include "assem.h"
#include "node.h"
#include "aerr.h"

extern int yyparse();
extern FILE *yyin, *yyout;

#define EMU_CMD_SIZE 80

int main(int argc, char* argv[])
{
	FILE* img = NULL;
	char emucmd[EMU_CMD_SIZE];
	int nerrors, emures;
	struct errinfo* errval;

	// Define arguments.
	struct arg_lit* show_help = arg_lit0("h", "help", "Show this help.");
	struct arg_lit* invoke_emulator = arg_lit0("e", NULL, "Invoke the emulator on the output automatically.");
	struct arg_file* input_file = arg_file1(NULL, NULL, "<file>", "The input file.");
	struct arg_file* output_file = arg_file1("o", "output", "<file>", "The output file.");
	struct arg_end *end = arg_end(20);
	void *argtable[] = { output_file, show_help, invoke_emulator, input_file, end };

	// Parse arguments.
	nerrors = arg_parse(argc,argv,argtable);
	if (nerrors != 0 || show_help->count != 0)
	{
		if (show_help->count != 0)
			arg_print_errors(stdout, end, "assembler");
		printf("\syntax:\n    assembler");
		arg_print_syntax(stdout, argtable, "\n");
		printf("\options:\n");
		arg_print_glossary(stdout, argtable, "    %-25s %s\n");
		exit(1);
	}

	// Set up error handling.
	errval = (struct errinfo*)setjmp(errjmp);
	if (errval != NULL)
	{
		// Handle the error.
		printf("assembler: internal error occurred.\n");
		printf(err_strings[errval->errid], errval->errdata);
		if (img != NULL)
			fclose(img);
		return 1;
	}

	// Parse assembly.
	yyin = fopen(input_file->filename[0], "r");
	if (yyin == NULL)
	{
		printf("assembler: input file not found.\n");
		return 1;
	}
	yyparse();
	fclose(yyin);
	
	// Process AST.
	process_root(&ast_root);

	// Write to file.
	img = fopen(output_file->filename[0], "wb");
	if (img == NULL)
	{
		printf("assembler: output file not writable.\n");
		return 1;
	}
	aout_write(img);
	fclose(img);
	printf("assembler: completed successfully.\n");

	// Execute emulator if desired.
	if (invoke_emulator->count > 0)
	{
		memset(emucmd, 0, EMU_CMD_SIZE);
		strcat(emucmd, "emulator \"");
		strcat(emucmd, output_file->filename[0]);
		strcat(emucmd, "\"");
		printf("assembler: executing %s", emucmd);
		emures = system(emucmd);
		if (emures == 0)
			printf("assembler: emulator exited successfully.\n");
		else
			printf("assembler: emulator exited with code %i.\n", emures);
	}
	
	return 0;
}