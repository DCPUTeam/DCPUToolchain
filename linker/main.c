/**

	File:			main.c

	Project:		DCPU-16 Tools
	Component:		Linker

	Authors:		James Rhodes

	Description:	Main entry point.

**/

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <argtable2.h>
#include "ldata.h"
#include "lprov.h"

const char objfmt[] = "OBJECT-FORMAT-1.0";
struct lprov_entry* provided = NULL;

void objfile_load(const char* filename, FILE* in)
{
	struct ldata_entry* entry = NULL;
	struct lprov_entry* last = NULL;
	struct lprov_entry* current = NULL;
	uint32_t offset = 0;
	size_t sz;

	// Read <256 byte label content> <mode> <address>
	// until mode is LABEL_END.
	//
	// If address is LABEL_REQUIRED, then it's a label that
	// needs resolving, otherwise it's a label that is
	// provided to other object files.
	entry = ldata_read(in);
	while (entry->label[0] != 0)
	{
		if (entry->mode == LABEL_END)
			break;
		else if (entry->mode == LABEL_PROVIDED)
		{
			current = lprov_create(entry->label, entry->address + offset);
			if (last == NULL)
				provided = current;
			else
				last->next = current;
			last = current;
		}

		entry = ldata_read(in);
	}

	// Now read the rest of the file to determine it's total
	// length so that we can adjust what will be the offset.
	sz = ftell(in);
	fseek(in, 0, SEEK_END);
	offset += 

	while (!feof(in))
		fseek(
}

int main(int argc, char* argv[])
{
	// Define our variables.
	FILE* in;
	FILE* out;
	int nerrors, i;
	char* test;

	// Define arguments.
	struct arg_lit* show_help = arg_lit0("h", "help", "Show this help.");
	struct arg_file* input_files = arg_filen(NULL, NULL, "<file>", 1, 0, "The input object files.");
	struct arg_file* output_file = arg_file1("o", "output", "<file>", "The output file (or - to send to standard output).");
	struct arg_end *end = arg_end(20);
	void *argtable[] = { show_help, input_files, output_file, end };
	
	// Parse arguments.
	nerrors = arg_parse(argc,argv,argtable);
	if (nerrors != 0 || show_help->count != 0)
	{
		if (show_help->count != 0)
			arg_print_errors(stdout, end, "linker");
		printf("syntax:\n    linker");
		arg_print_syntax(stdout, argtable, "\n");
		printf("options:\n");
		arg_print_glossary(stdout, argtable, "    %-25s %s\n");
		return 1;
	}

	// Open the output file for writing.
	out = fopen(output_file->filename[0], "wb");
	if (out == NULL)
	{
		// Handle the error.
		fprintf(stderr, "linker: unable to write to output file.\n");
		return 1;
	}

	// Loop through each input file.
	for (i = 0; i < input_files->count; i++)
	{
		// Open the input file.
		in = fopen(input_files->filename[i], "rb");
		if (in == NULL)
		{
			// Handle the error.
			fprintf(stderr, "linker: unable to read input file '%s'.\n", input_files->filename[i]);
			fclose(out);
			return 1;
		}
		
		// Is this the object format?
		test = malloc(strlen(objfmt) + 1);
		memset(test, 0, strlen(objfmt) + 1);
		fread(test, 1, strlen(objfmt), in);
		if (strcmp(test, objfmt) != 0)
		{
			// Handle the error.
			fprintf(stderr, "linker: input file '%s' is not in object format 1.0.\n", input_files->filename[i]);
			fclose(in);
			fclose(out);
			return 1;
		}
		free(test);

		// Load the object file's entries into memory.
		objfile_load(input_files->filename[i], in);

		// Close the file.
		fclose(in);
	}

	// Close file.
	fprintf(stderr, "linker: completed successfully.\n", input_files->filename[i]);
	fclose(out);

	return 0;
}

