%{

///
/// @file
/// @brief Lexer for preprocessor test suite.
///

#include <lexfix.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <bstring.h>
#include <rparse.h>
#include "parser.h"

// Filename tracking for local assembly.
bstring yyfilename = NULL;

%}

%option noyywrap
%option yylineno

%%

    /* Line and file directives */
    
\#\ [0-9]+(\r\n|\r|\n)                  rparse_dol(yytext, &yylineno);
\#\ [0-9]+\ [^\r\n]*(\r\n|\r|\n)        rparse_do(yytext, &yylineno, &yyfilename);

    /* Lexer */
    
[0-9]+          yylval.number = strtoul(yytext, NULL, 10); return NUMBER;
(\r\n|\r|\n)    return NEWLINE;
[ \t]+          /* ignore whitespace */;

%%

///
/// @}
///