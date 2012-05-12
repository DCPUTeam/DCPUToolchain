/**

	File:		main.c

	Project:	DCPU-16 Tools
	Component:	Linker

	Authors:	James Rhodes

	Description:	Main entry point.

**/

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <argtable2.h>
#include <version.h>
#include "objfile.h"
#include "lprov.h"
#include "ldata.h"

int main(int argc, char* argv[])
{
	// Define our variables.
	FILE* in;
	FILE* out;
	int nerrors, i;
	char* test;
	uint16_t offset, current, store, mem_index;
	struct lprov_entry* required = NULL;
	struct lprov_entry* provided = NULL;
	struct lprov_entry* adjustment = NULL;
	struct lprov_entry* temp = NULL;

	// Define arguments.
	struct arg_lit* show_help = arg_lit0("h", "help", "Show this help.");
	struct arg_file* input_files = arg_filen(NULL, NULL, "<file>", 1, 100, "The input object files.");
	struct arg_file* output_file = arg_file1("o", "output", "<file>", "The output file (or - to send to standard output).");
	struct arg_end* end = arg_end(20);
	void* argtable[] = { show_help, input_files, output_file, end };

	// Parse arguments.
	nerrors = arg_parse(argc, argv, argtable);

	version_print(bautofree(bfromcstr("Linker")));
	if (nerrors != 0 || show_help->count != 0)
	{
		if (show_help->count != 0)
			arg_print_errors(stdout, end, "linker");

		printf("syntax:\n    dtld");
		arg_print_syntax(stdout, argtable, "\n");
		printf("options:\n");
		arg_print_glossary(stdout, argtable, "	  %-25s %s\n");
		arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
		return 1;
	}

	// Open the output file for writing.
	out = fopen(output_file->filename[0], "wb");

	if (out == NULL)
	{
		// Handle the error.
		fprintf(stderr, "linker: unable to write to output file.\n");
		arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
		return 1;
	}

	// We initially need to get a list of ALL provided
	// labels before we can start replacing them.
	offset = 0;

	for (i = 0; i < input_files->count; i++)
	{
		// Open the input file.
		in = fopen(input_files->filename[i], "rb");

		if (in == NULL)
		{
			// Handle the error.
			fprintf(stderr, "linker: unable to read input file '%s'.\n", input_files->filename[i]);
			fclose(out);
			arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
			return 1;
		}

		// Is this the object format?
		test = malloc(strlen(ldata_objfmt) + 1);
		memset(test, 0, strlen(ldata_objfmt) + 1);
		fread(test, 1, strlen(ldata_objfmt), in);
		fseek(in, 1, SEEK_CUR);

		if (strcmp(test, ldata_objfmt) != 0)
		{
			// Handle the error.
			fprintf(stderr, "linker: input file '%s' is not in object format 1.0.\n", input_files->filename[i]);
			fclose(in);
			fclose(out);
			arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
			return 1;
		}

		free(test);

		// Load only the provided label entries into memory.
		objfile_load(input_files->filename[i], in, &offset, &provided, NULL, NULL);

		// Close the file.
		fclose(in);
	}

	// Now we can start replacing the labels with the provided values
	// since we have ALL of the provided labels available.
	offset = 0;

	for (i = 0; i < input_files->count; i++)
	{
		// Open the input file.
		in = fopen(input_files->filename[i], "rb");

		if (in == NULL)
		{
			// Handle the error.
			fprintf(stderr, "linker: unable to read input file '%s'.\n", input_files->filename[i]);
			fclose(out);
			arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
			return 1;
		}

		// Skip over the object format label; we already tested
		// for this in phase 1.
		fseek(in, strlen(ldata_objfmt) + 1, SEEK_CUR);

		// Load only the required and adjustment entries into memory.
		current = offset;
		objfile_load(input_files->filename[i], in, &offset, NULL, &required, &adjustment);

		// Copy all of the input file's data into the output
		// file, word by word.
		mem_index = 0;
		fprintf(stderr, "BEGIN %s\n", input_files->filename[i]);

		while (!feof(in))
		{
			// Read a word.
			fread(&store, sizeof(uint16_t), 1, in);

			// For some strange reason, the last two bytes get
			// written twice, as if it's only EOF after you
			// attempt to read past the end again.	I'm not sure
			// why the semantics are like this, but checking again
			// for EOF here prevents us writing double.
			if (feof(in))
				break;

			// Check to see if we need to do something with this
			// word, such as adjusting it.
			if (lprov_find_by_address(adjustment, mem_index) != NULL)
			{
				// We need to adjust this word by the offset.
				store += current;
				fprintf(stderr, "ADJUSTED 0x%04X: 0x%04X -> 0x%04X\n", mem_index, store - current, store);
			}

			// Check to see if we need to resolve this word into
			// an actual address because it was imported.
			temp = lprov_find_by_address(required, mem_index);

			if (temp != NULL)
			{
				// Find the position we should change this to.
				temp = lprov_find_by_label(provided, temp->label);

				// Ensure that temp is not NULL, if it is, then we couldn't
				// resolve this entry.
				if (temp == NULL)
				{
					temp = lprov_find_by_address(required, mem_index);
					fprintf(stderr, "NOT FOUND! 0x%04X: 0x%04X -> %s ???\n", mem_index, store, temp->label);
					fclose(out);
					arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
					return 1;
				}

				// We need to set this word to the proper location.
				fprintf(stderr, "RESOLVED 0x%04X: 0x%04X -> 0x%04X\n", mem_index, store, temp->address);
				store = temp->address;
			}

			// Now write the (potentially modified) word to the
			// output.
			fprintf(stderr, " >> WRITE 0x%04X\n", store);
			fwrite(&store, sizeof(uint16_t), 1, out);

			// Increment memory position.
			mem_index++;
		}

		// Close the file.
		fclose(in);

		// Reset and free the required and adjustment linked list.
		// FIXME: Actually free the lists!
		required = NULL;
		adjustment = NULL;
	}

	// Close file.
	fprintf(stderr, "linker: completed successfully.\n", input_files->filename[i]);
	fclose(out);

	arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
	return 0;
}

