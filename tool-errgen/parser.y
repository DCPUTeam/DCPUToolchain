%{

/**

	File:           parser.y

	Project:        DCPU-16 Tools
	Component:      Tool-ErrGen

	Authors:        James Rhodes

	Description:    Defines parser for the error definition generator.

**/

#define YYERROR_VERBOSE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <simclist.h>

extern FILE* yyin;
extern FILE* yyout;
extern int yylex();
void yyerror(const char* str);
list_t warnings;
list_t errors;

struct entry
{
    const char* def;
    const char* name;
    const char* message;
};

%}

// Defines a union for storing all of our data types.
%union
{
	char* word;
    int token;
}

// Define our lexical token names.
%token <token> NEWLINE
%token <word> ERROR_DEF WARNING_DEF NAME MESSAGE

// Start at the root node.
%start input

%%

input: definitions ;

definitions:
        /* empty */
        {
        } |
        definitions ERROR_DEF NAME MESSAGE NEWLINE
        {
            struct entry* e = malloc(sizeof(struct entry));
            e->def = $2;
            e->name = $3;
            e->message = $4;
            list_append(&errors, e);
        } |
        definitions WARNING_DEF NAME MESSAGE NEWLINE
        {
            struct entry* e = malloc(sizeof(struct entry));
            e->def = $2;
            e->name = $3;
            e->message = $4;
            list_append(&warnings, e);
        } |
        definitions NEWLINE
        {
        } ;

%%

void yyerror(const char* str)
{
    fprintf(stderr, "error: parsing definition file failed!\n");
}

int main(int argc, char** argv)
{
    FILE* input;
    FILE* header;
    FILE* source;
    int i;
    int error_count = 0;
    int warning_count = 0;
    struct entry* e;

    // Check arguments.
    if (argc != 4)
    {
        fprintf(stderr, "usage: tool-errgen <input> <output-header> <output-source>");
        return 1;
    }

    // Open files.
    input = fopen(argv[1], "r");
    header = fopen(argv[2], "w");
    source = fopen(argv[3], "w");
    if (input == NULL || header == NULL || source == NULL)
    {
        if (input != NULL) fclose(input);
        if (header != NULL) fclose(header);
        if (source != NULL) fclose(source);
        fprintf(stderr, "error: unable to open input or outputs files.\n");
        return 1;
    }

    // Initialize lists.
    list_init(&errors);
    list_init(&warnings);

    // Assign input / outputs and parse.
    yyout = stderr;
	yyin = input;
	yyparse();

    // Generate header code based on results.
    i = 0;
    fprintf(header, "#ifndef __DCPU_ERRORS\n");
    fprintf(header, "#define __DCPU_ERRORS\n");
    fprintf(header, "#include \"derr.h\"\n"); // FIXME
    list_iterator_start(&errors);
    while (list_iterator_hasnext(&errors))
    {
        e = list_iterator_next(&errors);
        fprintf(header, "#define %s %i\n", e->def, i++);
        error_count++;
    }
    list_iterator_stop(&errors);
    list_iterator_start(&warnings);
    while (list_iterator_hasnext(&warnings))
    {
        e = list_iterator_next(&warnings);
        fprintf(header, "#define %s %i\n", e->def, i++);
        warning_count++;
    }
    list_iterator_stop(&warnings);
    fprintf(header, "#define _WARN_OFFSET %i\n", error_count);
    fprintf(header, "#define _ERR_COUNT %i\n", error_count + warning_count);
    fprintf(header, "#define _WARN_COUNT (_ERR_COUNT - _WARN_OFFSET)\n");
    fprintf(header, "extern const char* derrstr[%i];\n", error_count + warning_count);
    fprintf(header, "extern struct warnpolicy dwarnpolicy[%i];\n", warning_count);
    fprintf(header, "#endif\n");

    // Generate source code based on results.
    fprintf(source, "#include \"derr.h\"\n"); // FIXME
    fprintf(source, "const char* derrstr[%i] =\n", error_count + warning_count);
    fprintf(source, "{\n");
    list_iterator_start(&errors);
    while (list_iterator_hasnext(&errors))
    {
        e = list_iterator_next(&errors);
        fprintf(source, "    \"%s\\n\",\n", e->message);
    }
    list_iterator_stop(&errors);
    list_iterator_start(&warnings);
    while (list_iterator_hasnext(&warnings))
    {
        e = list_iterator_next(&warnings);
        fprintf(source, "    \"%s\\n\",\n", e->message);
    }
    list_iterator_stop(&warnings);
    fprintf(source, "};\n");
    fprintf(source, "struct warnpolicy dwarnpolicy[%i] =\n", warning_count);
    fprintf(source, "{\n");
    list_iterator_start(&warnings);
    while (list_iterator_hasnext(&warnings))
    {
        e = list_iterator_next(&warnings);
        fprintf(source, "    { %s, \"%s\", 0, 0 },\n", e->def, e->name);
    }
    list_iterator_stop(&warnings);
    fprintf(source, "};\n");

    // Close files.
    fclose(source);
    fclose(header);
    fclose(input);

    // Done.
    return 0;
}

