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

#include <stdio.h>
#include <bstrlib.h>
#include <bstraux.h>
#include <simclist.h>
extern FILE* yyin;
FILE* yyout;
extern int yylineno;
list_t equates;

void pp_discard_buffer();

void yyerror(void* scanner, const char *str)
{
    fprintf(stderr,"error at line %i: %s\n", yylineno, str);
}

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
	list_init(&equates);
	list_attributes_copy(&equates, equate_entry_meter, 1);
	list_attributes_seeker(&equates, equate_entry_seeker);
};

// We need a reentrant parser to do #include.
%pure-parser
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
			
			fprintf(yyout, "%s", $1->data);
			bdestroy($1);
		} ;

preprocessor:
		INCLUDE STRING
		{
			/* We must recursively invoke the preprocessor on the new
			   input, sending to standard output. */
			FILE* yyold;
			fprintf(stderr, "Found .INCLUDE.");
			yyold = yyin;
			yyin = fopen($2->data, "r");
			fprintf(stderr, "Switched yyin.");
			pp_discard_buffer(); /* Discard current lexer buffer. */
			fprintf(stderr, "Discarded buffer.");
			fprintf(stderr, "Starting parsing...");
			yyparse(0);
			fprintf(stderr, "Finished parsing.");
			fclose(yyin);
			fprintf(stderr, "Closed file.");
			yyin = yyold;
			fprintf(stderr, "Set yyin back again.");
			pp_discard_buffer(); /* Discard current lexer buffer again. */
			fprintf(stderr, "Discarded buffer again.");
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
