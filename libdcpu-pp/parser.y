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
list_t scopes;
list_t macros;
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

struct scope_entry
{
	bool active;
};

struct macrodef_entry
{
	bstring name;
	list_t args;
	bstring content;
};

struct macroarg_entry
{
	bstring string;
	int number;
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

size_t macro_entry_meter(const void* el)
{
	return sizeof(struct macrodef_entry);
}

int macro_entry_seeker(const void* el, const void* key)
{
	if (biseq((bstring)key, ((struct macrodef_entry*)el)->name))
		return 1;
	return 0;
}

struct scope_entry* active_scope = NULL;
struct macrodef_entry* active_macro = NULL;

%}

// Defines a union for storing all of our data types.
%union
{
	int number;
	bstring string;
	char any;
	int* token;
	struct macrodef_entry* macro;
	struct macroarg_entry* arg;
	list_t list;
}

// Define our lexical token names.
%token <token> INCLUDE EQUATE IF IFDEF IFNDEF ELSE ENDIF MACRO ENDMACRO PARAM_OPEN PARAM_CLOSE COMMA UNDEF MACROCALL
%token <string> STRING WORD
%token <any> ANY_CHAR
%token <number> NUMBER

// Define our types.
%type <token> lines line preprocessor
%type <string> text
%type <macro> macrodef
%type <arg> callarg
%type <list> macrodefargs macrocallargs

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
		list_init(&scopes);
		list_init(&macros);
		list_attributes_copy(&equates, equate_entry_meter, 1);
		list_attributes_copy(&macros, macro_entry_meter, 1);
		list_attributes_seeker(&equates, equate_entry_seeker);
		list_attributes_seeker(&macros, macro_entry_seeker);
		active_scope = malloc(sizeof(struct scope_entry));
		active_scope->active = true;
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

			// Skip replacement if we are inside a macro.
			if (active_macro == NULL)
			{
				// Do substring replacements for equated directives.
				for (i = 0; i < list_size(&equates); i++)
				{
					e = (struct equate_entry*)list_get_at(&equates, i);
					bfindreplace($1, e->name, e->replace, 0);
				}
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
		} |
		macrodef
		{
			if ($1 != NULL)
			{
				// Set our active macro definition.
				active_macro = $1;
			}
			else
			{
				// TODO: Throw an error here because we're trying to define
				//       a macro inside another macro, which is not permitted.
				//       (note that if macrodef handles this, then the if clause
				//       is no longer required at all).
			}
		} |
		ENDMACRO
		{
			// We have finished defining the macro.  Put it into our
			// list of macros that are defined and clear the active_macro
			// state.
			list_append(&macros, active_macro);
			active_macro = NULL;
		} |
		MACROCALL WORD PARAM_OPEN macrocallargs PARAM_CLOSE
		{
			unsigned int i, a;
			struct macrodef_entry* m;
			struct macroarg_entry* arg;
			bstring temp;
			bstring argname;
			FILE* tfile;

			// Evaluate the macro that need evaluating.
			for (i = 0; i < list_size(&macros); i++)
			{
				m = (struct macrodef_entry*)list_get_at(&macros, i);
				if (biseq(m->name, $2))
				{
					// Check to make sure the arguments passed and
					// arguments required are the same size.
					if (list_size(&$4) != list_size(&m->args))
					{
						// Throw an error here about the parameters
						// not being the required amount.
					}
					else
					{
						// We need to write out a temporary file; Yuck!
						// We output all of the parameters as definitions,
						// followed by the text, then .UNDEFs to remove them
						// from scope.
						temp = bfromcstr(tempnam(".", "pp.macro."));
						tfile = fopen((const char*)(temp->data), "w");
						if (tfile == NULL)
						{
							// TODO: Throw an error here because we were unable
							// to output the macro to a file.
						}
						else
						{
							// Write out each of the parameters.
							for (a = 0; a < list_size(&m->args); a++)
							{
								argname = (bstring)list_get_at(&m->args, a);
								arg = (struct macroarg_entry*)list_get_at(&$4, a);
								if (arg->string != NULL)
									fprintf(tfile, ".EQUATE %s \"%s\"\n", argname->data, arg->string->data);
								else
									fprintf(tfile, ".EQUATE %s %i\n", argname->data, arg->number);
							}

							// Write out the content.
							fprintf(tfile, "%s\n", m->content->data);

							// Write out each of the undefine commands.
							for (a = 0; a < list_size(&m->args); a++)
							{
								argname = (bstring)list_get_at(&m->args, a);
								arg = (struct macroarg_entry*)list_get_at(&$4, a);
								if (arg->string != NULL)
									fprintf(tfile, ".UNDEF %s\n", argname->data, arg->string->data);
								else
									fprintf(tfile, ".UNDEF %s\n", argname->data, arg->number);
							}

							// Write out another newline and then close the file.
							fprintf(tfile, "\n");
							fflush(tfile);
							fclose(tfile);

							// Now we invoke the file as if it was an include.
							handle_include(temp, scanner);

							// Now remove the file and cleanup.
							// TODO: Free any used memory here.
							remove((const char*)(temp->data));
						}
					}
				}
			}

			// TODO: Probably should error if we get to here, since this means
			// no macro was evaluated.
		} |
		IFDEF WORD
		{
			unsigned int i;
			struct equate_entry* e;

			// First push our existing scope onto the list and create
			// a new scope.
			list_append(&scopes, active_scope);
			active_scope = malloc(sizeof(struct scope_entry));
			active_scope->active = false;

			// Check to see whether the specified equate exists.
			for (i = 0; i < list_size(&equates); i++)
			{
				e = (struct equate_entry*)list_get_at(&equates, i);
				if (biseq(e->name, $2))
				{
					// Equate found.
					active_scope->active = true;
					break;
				}
			}
		} |
		IFNDEF WORD
		{
			unsigned int i;
			struct equate_entry* e;

			// First push our existing scope onto the list and create
			// a new scope.
			list_append(&scopes, active_scope);
			active_scope = malloc(sizeof(struct scope_entry));
			active_scope->active = true;

			// Check to see whether the specified equate exists.
			for (i = 0; i < list_size(&equates); i++)
			{
				e = (struct equate_entry*)list_get_at(&equates, i);
				if (biseq(e->name, $2))
				{
					// Equate found.
					active_scope->active = false;
					break;
				}
			}
		} |
		UNDEF WORD
		{
			// TODO: Implement this!
		} |
		ELSE
		{
			// Switch the current silencing mode.
			active_scope->active = !active_scope->active;
		} |
		ENDIF
		{
			struct scope_entry* result;

			// Check to make sure we have an outer-scope.
			if (list_size(&scopes) > 0)
			{
				// Exit this scope.
				free(active_scope);
				result = list_extract_at(&scopes, list_size(&scopes) - 1);
				active_scope = result;
			}
			else
			{
				// TODO: Throw an error here because we encountered
				// one more .ENDIF than we should have.
			}
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

macrodef:
		MACRO WORD PARAM_OPEN macrodefargs PARAM_CLOSE
		{
			if (active_macro != NULL)
			{
				// TODO: Throw an error here because we're trying to define
				//       a macro inside another macro, which is not permitted.
				$$ = NULL;
			}
			else
			{
				// Define the macro's header.
				struct macrodef_entry* entry = malloc(sizeof(struct macrodef_entry));
				entry->name = $2;
				entry->args = $4;
				entry->content = bfromcstr("");
				$$ = entry;
			}
		} ;

macrodefargs:
		WORD
		{
			list_init(&$$);
			list_append(&$$, $1);
		} |
		macrodefargs COMMA WORD
		{
			$$ = $1;
			list_append(&$$, $3);
		} ;

macrocallargs:
		callarg
		{
			list_init(&$$);
			list_append(&$$, $1);
		} |
		macrocallargs COMMA callarg
		{
			$$ = $1;
			list_append(&$$, $3);
		} ;

callarg:
		WORD
		{
			$$ = malloc(sizeof(struct macroarg_entry));
			$$->string = $1;
		} |
		STRING
		{
			$$ = malloc(sizeof(struct macroarg_entry));
			$$->string = $1;
		} |
		NUMBER
		{
			$$ = malloc(sizeof(struct macroarg_entry));
			$$->number = $1;
		};

%%

#include "lexer.h"

void yyerror(void* scanner, const char *str)
{
    fprintf(stderr,"error at line %i: %s\n", pp_yyget_lineno(scanner), str);
}

void handle_output(bstring output, void* scanner)
{
	if (active_scope->active && active_macro == NULL)
		fprintf(pp_yyget_out(scanner), "%s", output->data);
	else if (active_scope->active && active_macro != NULL)
	{
		bconcat(active_macro->content, output);
		bconchar(active_macro->content, '\n');
	}
}

void handle_include(bstring fname, void* current)
{
	yyscan_t scanner;
	FILE* ffnew;
	bstring dir;
	bstring path;

	// Search for the file using ppfind.
	if (fname->data[0] != '/' && fname->data[1] != ':')
		path = ppfind_locate(bautocpy(fname));
	else
		path = bstrcpy(fname);
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
	ppfind_add_path(bautocpy(dir));

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
	ppfind_remove_path(bautocpy(dir));
}
