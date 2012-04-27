%{
	
/**

	File:			parser.y

	Project:		DCPU-16 Tools
	Component:		LibDCPU-PP

	Authors:		James Rhodes

	Description:	Defines parser for the preprocessor.

**/

#define YYDEBUG 1
#define YYERROR_VERBOSE

#include <lexfix.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <bstrlib.h>
#include <bstraux.h>
#include <simclist.h>
#include <osutil.h>
#include "ppfind.h"
#include "dcpu.h"
list_t equates;
void pp_discard_buffer();
void yyerror(void* scanner, const char *str);
void handle_output(bstring output, void* scanner);
void handle_include(bstring fname, void* current);
bool is_globally_initialized = false;

struct equate_entry
{
	bstring name;
	bstring replace;
};

size_t equate_entry_meter(const void* el)
{
	return sizeof(struct equate_entry);
}

int equate_entry_seeker(const void* el, const void* key)
{
	if (biseq((bstring)key, ((struct equate_entry*)el)->name))
		return 1;
	return 0;
}

%}

// Defines a union for storing all of our data types.
%union
{
	int number;
	bstring string;
	char any;
	int* token;
}

// Define our lexical token names.
%token <token> INCLUDE EQUATE
%token <string> STRING WORD
%token <any> ANY_CHAR
%token <number> NUMBER

// Define our types.
%type <token> lines line preprocessor
%type <string> text

// Start at the root node.
%start root

// Initialize everything.
%initial-action
{
	// We don't want to reinitialize our equates
	// list when entering new files (via includes).
	if (!is_globally_initialized)
	{
		list_init(&equates);
		list_attributes_copy(&equates, equate_entry_meter, 1);
		list_attributes_seeker(&equates, equate_entry_seeker);
		is_globally_initialized = true;
	}
};

// We need a reentrant parser to do #include.
%pure-parser
%name-prefix="pp_yy"
%lex-param {void* scanner}
%parse-param {void* scanner}

%%

root:
		lines;

lines:
		line |
		lines line ;

line:
		preprocessor |
		text
		{
			unsigned int i;
			struct equate_entry* e;

			// Do substring replacements for equated directives.
			for (i = 0; i < list_size(&equates); i++)
			{
				e = (struct equate_entry*)list_get_at(&equates, i);
				bfindreplace($1, e->name, e->replace, 0);
			}
			
			handle_output($1, scanner);
			bdestroy($1);
		} ;

preprocessor:
		INCLUDE STRING
		{
			/* We must recursively invoke the preprocessor on the new
			   input, sending to standard output. */
			handle_include($2, scanner);
		} |
		EQUATE WORD STRING
		{
			struct equate_entry* entry = malloc(sizeof(struct equate_entry));
			entry->name = $2;
			entry->replace = $3;
			list_append(&equates, entry);
		} ;

text:
		ANY_CHAR
		{
			$$ = bfromcstralloc(1, " ");
			$$->data[0] = $1;
		} | 
		text ANY_CHAR
		{
			bstring s = bfromcstralloc(1, " ");
			s->data[0] = $2;
			bconcat($$, s);
			bdestroy(s);
		} ;

%%

#include "lexer.h"

void yyerror(void* scanner, const char *str)
{
    fprintf(stderr,"error at line %i: %s\n", pp_yyget_lineno(scanner), str);
}

void handle_output(bstring output, void* scanner)
{
	fprintf(pp_yyget_out(scanner), "%s", output->data);
}

void handle_include(bstring fname, void* current)
{
	yyscan_t scanner;
	FILE* ffnew;
	bstring dir;

	// Search for the file using ppfind.
	bstring path = ppfind_locate(fname);
	if (path == NULL)
	{
		// TODO: We need error handling the preprocessor
		//       so we can pass errors back like 'include not found'.
		//       Maybe we can model this off the ahalt() mechanism
		//       in the assembler.
		fprintf(stderr, "can't include '%s'.", fname->data);
		return;
	}
	
	// Calculate the directory that the include
	// file is located in and add that directory
	// to the include path.
	dir = osutil_dirname(path);
	ppfind_add_path(dir);

	// Include the file.
	ffnew = fopen((const char*)(path->data), "r");
	assert(ffnew != NULL);
	pp_yylex_init(&scanner);
	pp_yyset_in(ffnew, scanner);
	pp_yyset_out(pp_yyget_out(current), scanner);
	pp_yyparse(scanner);
	fclose(ffnew);
	pp_yylex_destroy(scanner);
	bdestroy(path);

	// Remove the include path again.
	ppfind_remove_path(dir);
}