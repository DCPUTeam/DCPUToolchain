%{

///
/// @file
/// @brief Preprocessor for preprocessor test suite.
///

#define YYERROR_VERBOSE

#include <bstring.h>
#include <stdio.h>
#include <dcpu.h>

extern int yylex();
void yyerror(const char *str);

// Filename tracking for local assembly.
extern int  yylineno;
extern bstring  yyfilename;

// Success result.
extern int test_success;

%}

%union
{
    int number;
    int* token;
}

// Define our lexical token names.
%token <token> NEWLINE
%token <number> NUMBER

// Start at the root node.
%start root

%%

root:
        lines
        {
        } ;
        
lines:
        line
        {
        } |
        lines line
        {
        } ;
        
line:
        NEWLINE
        {
            // something
        } |
        NUMBER
        {
            // Verify current line with the number value.
            if ($1 == yylineno)
            {
                printf("line %i is correct.\n", yylineno);
            }
            else
            {
                printf("FAILURE AT LINE %i (expected %i)!\n", yylineno, $1);
                test_success = false;
            }
        } ;
        
%%

void yyerror(const char *str)
{
    printf("FAILURE: %s\n", str);
    test_success = false;
}