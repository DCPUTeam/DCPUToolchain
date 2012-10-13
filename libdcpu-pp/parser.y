%{

/**

	File:           parser.y

	Project:        DCPU-16 Tools
	Component:      LibDCPU-pp

	Authors:        James Rhodes

	Description:    Defines parser for the preprocessor.

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
#include <lua.h>
#include <osutil.h>
#include <debug.h>
#include "pplua.h"
#include "ppfind.h"
#include "ppexpr.h"
#include "dcpu.h"

list_t equates;
list_t scopes;
list_t macros;
struct pp_state lstate;
void pp_discard_buffer();
void yyerror(void* scanner, const char *str);
void handle_uline(int line, bstring file, void* current);
void handle_line(int line, bstring file, void* current);
void handle_start(freed_bstring name, FILE* current);
void handle_output(bstring output, void* scanner);
int handle_include(bstring fname, void* current);
void handle_pp_lua_print(const char* text, void* current);
void handle_pp_lua_print_line(const char* text, void* current);
void handle_pp_lua_scope_enter(bool output, void* current);
void handle_pp_lua_scope_exit(void* current);
void handle_pp_lua_add_symbol(const char* symbol, void* current);
void finalize_lua(int line, void* current);
uint16_t equate_replace(bstring label);
void equate_errexit(int code, void* data);
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
	struct expr* expr;
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
	struct customarg_entry* carg;
	struct expr* expr;
	list_t list;
}

// Define our lexical token names.
%token <token> ADD SUBTRACT MULTIPLY DIVIDE MODULUS EQUALS NOT_EQUALS LESS_THAN LESS_EQUALS GREATER_THAN GREATER_EQUALS
%token <token> PAREN_OPEN PAREN_CLOSE BITWISE_AND BITWISE_BOR BITWISE_XOR BITWISE_NOT BOOLEAN_OR BOOLEAN_AND BINARY_LEFT_SHIFT BINARY_RIGHT_SHIFT
%token <token> LINE ULINE INCLUDE EQUATE IF IFDEF IFNDEF ELSE ENDIF MACRO
%token <token> ENDMACRO PARAM_OPEN PARAM_CLOSE COMMA UNDEF MACROCALL
%token <string> STRING WORD CUSTOM
%token <any> WHITESPACE NEWLINE TEXT
%token <number> NUMBER

// Define our types.
%type <token> lines line preprocessor
%type <string> text
%type <macro> macrodef
%type <arg> callarg
%type <carg> customarg
%type <list> macrodefargs macrocallargs customargs
%type <expr> expr

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

		// Initialize Lua.
		lstate.pp_lua_print = &handle_pp_lua_print;
		lstate.pp_lua_print_line = &handle_pp_lua_print_line;
		lstate.pp_lua_scope_enter = &handle_pp_lua_scope_enter;
		lstate.pp_lua_scope_exit = &handle_pp_lua_scope_exit;
		lstate.pp_lua_add_symbol = &handle_pp_lua_add_symbol;
		pp_lua_init();
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
		NEWLINE
		{
			handle_output(bfromcstr("\n"), scanner);
		} |
		preprocessor NEWLINE |
		text NEWLINE
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
			handle_output(bfromcstr("\n"), scanner);
			bdestroy($1);
		} ;

preprocessor:
		INCLUDE STRING
		{
			/* We must recursively invoke the preprocessor on the new
			   input, sending to standard output. */
			if (!handle_include($2, scanner))
			{
				YYERROR;
			}
		} |
		LINE NUMBER STRING
		{
			handle_line($2, $3, scanner);
		} |
		LINE NUMBER WORD
		{
			handle_line($2, $3, scanner);
		} |
		ULINE NUMBER STRING
		{
			handle_uline($2, $3, scanner);
		} |
		ULINE NUMBER WORD
		{
			handle_uline($2, $3, scanner);
		} |
		EQUATE WORD
		{
			struct equate_entry* entry = malloc(sizeof(struct equate_entry));
			entry->name = $2;
			entry->replace = bfromcstr("");
			list_append(&equates, entry);
		} |
		EQUATE WORD text
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
									fprintf(tfile, ".UNDEF %s\n", argname->data);
								else
									fprintf(tfile, ".UNDEF %s\n", argname->data);
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
		IF expr
		{
			uint16_t result;

			// First push our existing scope onto the list and create
			// a new scope.
			list_append(&scopes, active_scope);
			active_scope = malloc(sizeof(struct scope_entry));
			active_scope->active = false;

			// Evaluate the expression.
			result = expr_evaluate($2, &equate_replace, &equate_errexit);
			
			// If the result was true, activate this scope.
			if (result)
				active_scope->active = true;
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
			unsigned int i;
			struct equate_entry* e;
			
			// Check to see whether the specified equate exists.
			for (i = 0; i < list_size(&equates); i++)
			{
				e = (struct equate_entry*)list_get_at(&equates, i);
				if (biseq(e->name, $2))
				{
					// Equate found.  Remove it.
					list_extract_at(&equates, i);
					i--;
				}
			}
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
		} |
		CUSTOM customargs
		{
			// Pass this off to the Lua preprocessor module system.
			int lineno = pp_yyget_lineno(scanner);
			pp_lua_handle(&lstate, scanner, $1, &$2);
			
			// Output line information to readjust for the fact that
			// the custom Lua module may have outputted more lines or
			// no lines at all.
			finalize_lua(lineno - 1, scanner);
		};
		
customargs:
		/* empty */
		{
			list_init(&$$);
		} |
		customargs customarg
		{
			$$ = $1;
			list_append(&$$, $2);
		} ;

customarg:
		WORD
		{
			$$ = malloc(sizeof(struct customarg_entry));
			$$->expr = NULL;
			$$->word = $1;
			$$->string = NULL;
			$$->number = 0;
		} |
		STRING
		{
			$$ = malloc(sizeof(struct customarg_entry));
			$$->expr = NULL;
			$$->word = NULL;
			$$->string = $1;
			$$->number = 0;
		} |
		NUMBER
		{
			$$ = malloc(sizeof(struct customarg_entry));
			$$->expr = NULL;
			$$->word = NULL;
			$$->string = NULL;
			$$->number = $1;
		} |
		expr
		{
			$$ = malloc(sizeof(struct customarg_entry));
			$$->expr = $1;
			$$->word = NULL;
			$$->string = NULL;
			$$->number = 0;
		};

text:
		TEXT
		{
			$$ = bfromcstralloc(1, " ");
			$$->data[0] = $1;
		} | 
		text TEXT
		{
			bstring s = bfromcstralloc(1, " ");
			s->data[0] = $2;
			bconcat($$, s);
			bdestroy(s);
		};

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
			$$->expr = NULL;
			$$->string = $1;
			$$->number = 0;
		} |
		STRING
		{
			$$ = malloc(sizeof(struct macroarg_entry));
			$$->expr = NULL;
			$$->string = $1;
			$$->number = 0;
		} |
		NUMBER
		{
			$$ = malloc(sizeof(struct macroarg_entry));
			$$->expr = NULL;
			$$->string = NULL;
			$$->number = $1;
		} |
		expr
		{
			$$ = malloc(sizeof(struct macroarg_entry));
			$$->expr = $1;
			$$->string = NULL;
			$$->number = 0;
		} ;

expr:
		NUMBER
		{
			$$ = expr_new_number($1);
		} |
		WORD
		{
			$$ = expr_new_label(bautofree($1));
		} |
		PAREN_OPEN expr PAREN_CLOSE
		{
			$$ = $2;
		} |
		SUBTRACT expr
		{
			$$ = expr_new(expr_new_number(0), EXPR_OP_SUBTRACT, $2);
		} |
		BITWISE_NOT expr
		{
			$$ = expr_new(expr_new_number(0xFFFF), EXPR_OP_XOR, $2);
		} |
		expr DIVIDE expr
		{
			$$ = expr_new($1, EXPR_OP_DIVIDE, $3);
		} |
		expr MULTIPLY expr
		{
			$$ = expr_new($1, EXPR_OP_MULTIPLY, $3);
		} |
		expr SUBTRACT expr
		{
			$$ = expr_new($1, EXPR_OP_SUBTRACT, $3);
		} |
		expr ADD expr
		{
			$$ = expr_new($1, EXPR_OP_ADD, $3);
		} |
		expr MODULUS expr
		{
			$$ = expr_new($1, EXPR_OP_MODULUS, $3);
		} |
		expr BITWISE_AND expr
		{
			$$ = expr_new($1, EXPR_OP_AND, $3);
		} |
		expr BITWISE_XOR expr
		{
			$$ = expr_new($1, EXPR_OP_XOR, $3);
		} |
		expr BITWISE_BOR expr
		{
			$$ = expr_new($1, EXPR_OP_BOR, $3);
		} |
		expr EQUALS expr
		{
			$$ = expr_new($1, EXPR_OP_EQUALS, $3);
		} |
		expr NOT_EQUALS expr
		{
			$$ = expr_new($1, EXPR_OP_NOT_EQUALS, $3);
		} |
		expr LESS_THAN expr
		{
			$$ = expr_new($1, EXPR_OP_LESS_THAN, $3);
		} |
		expr LESS_EQUALS expr
		{
			$$ = expr_new($1, EXPR_OP_LESS_EQUALS, $3);
		} |
		expr GREATER_THAN expr
		{
			$$ = expr_new($1, EXPR_OP_GREATER_THAN, $3);
		} |
		expr GREATER_EQUALS expr
		{
			$$ = expr_new($1, EXPR_OP_GREATER_EQUALS, $3);
		} ;

%%

#include "lexer.h"

bstring pp_yyfilename = NULL;

uint16_t equate_replace(bstring label)
{
	unsigned int i;
	struct equate_entry* e;
	
	// Find the desired equate.
	for (i = 0; i < list_size(&equates); i++)
	{
		e = (struct equate_entry*)list_get_at(&equates, i);
		if (biseq(e->name, label))
		{
			// Equate found.
			return (uint16_t)strtol(e->replace->data, NULL, 0);
		}
	}
	
	equate_errexit(EXPR_EXIT_LABEL_NOT_FOUND, label->data);
	return 0;
}

void equate_errexit(int code, void* data)
{
	if (code == EXPR_EXIT_LABEL_NOT_FOUND)
		printd(LEVEL_ERROR, "error occurred while evaluating expression: unable to find equate '%s'.\n", (const char*)data);
	else if (code == EXPR_EXIT_DIVIDE_BY_ZERO)
		printd(LEVEL_ERROR, "error occurred while evaluating expression: divide by zero.\n");
	else
		printd(LEVEL_ERROR, "error occurred while evaluating expression: unknown error.\n");
}

void yyerror(void* scanner, const char *str)
{
	fprintf(stderr, "error at line %i of file %s: %s\n", pp_yyget_lineno(scanner), pp_yyfilename->data, str);
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

void handle_line(int line, bstring file, void* current)
{
	fprintf(pp_yyget_out((yyscan_t)current), "# %i %s\n", line, file->data);
}

void handle_uline(int line, bstring file, void* current)
{
	fprintf(pp_yyget_out((yyscan_t)current), "#U %i %s\n", line, file->data);
}

void finalize_lua(int line, void* current)
{
	fprintf(pp_yyget_out(current), "# %i\n", line);
}

void handle_start(freed_bstring name, FILE* output)
{
	fprintf(output, "# 1 %s\n", name.ref->data);
	pp_yyfilename = bstrcpy(name.ref);
	bautodestroy(name);
}

int handle_include(bstring fname, void* current)
{
	yyscan_t scanner;
	FILE* ffnew;
	bstring dir;
	bstring path;
	int origline;
	bstring origfile;
	int result;
	
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
		bstring fmt = bformat("can't include '%s'.", fname->data);
		char* fmtc = bstr2cstr(fmt, '0');
		yyerror(current, fmtc);
		bcstrfree(fmtc);
		bdestroy(fmt);
		return false;
	}
	
	// Calculate the directory that the include
	// file is located in and add that directory
	// to the include path.
	dir = osutil_dirname(path);
	ppfind_add_path(bautocpy(dir));
	
	// Check to make sure the original filename is set.
	assert(pp_yyfilename != NULL);
	
	// Store existing state (line number
	// and filename).
	origfile = bstrcpy(pp_yyfilename);
	origline = pp_yyget_lineno(current);
	
	// Output the new state.
	fprintf(pp_yyget_out(current), "\n# 1 %s\n", path->data);
	bassign(pp_yyfilename, path);
	
	// Include the file.
	ffnew = fopen((const char*)(path->data), "r");
	assert(ffnew != NULL);
	pp_yylex_init(&scanner);
	pp_yyset_in(ffnew, scanner);
	pp_yyset_out(pp_yyget_out(current), scanner);
	result = pp_yyparse(scanner);
	fclose(ffnew);
	pp_yylex_destroy(scanner);
	bdestroy(path);

	// Check to see if there were errors.
	if (result != 0)
		return false;
	
	// Restore the old state and output
	// a line adjustment.
	bdestroy(pp_yyfilename);
	pp_yyfilename = origfile;
	pp_yyset_lineno(origline, current);
	fprintf(pp_yyget_out(current), "\n# %i %s\n", origline, origfile->data);
	
	// Remove the include path again.
	ppfind_remove_path(bautocpy(dir));

	return true;
}

void handle_pp_lua_print(const char* text, void* current)
{
	fprintf(pp_yyget_out((yyscan_t)current), "%s", text);
}

void handle_pp_lua_print_line(const char* text, void* current)
{
	fprintf(pp_yyget_out((yyscan_t)current), "%s\n", text);
}

void handle_pp_lua_scope_enter(bool output, void* current)
{
}

void handle_pp_lua_scope_exit(void* current)
{
}

void handle_pp_lua_add_symbol(const char* symbol, void* current)
{
	fprintf(pp_yyget_out((yyscan_t)current), ".SYMBOL %s\n", symbol);
}
