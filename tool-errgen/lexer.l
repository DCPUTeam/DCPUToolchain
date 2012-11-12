%{

/**

	File:           lexer.y

	Project:        DCPU-16 Tools
	Component:      Tool-ErrGen

	Authors:        James Rhodes

	Description:    Defines lexer for the error definition generator.

**/

#ifdef _WIN32
#pragma warning(disable:4005;disable:4013;disable:4018)
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "parser.h"

%}

%option noyywrap

%x STRING

%%

ERR_[A-Z0-9_]+  yylval.word = strdup(yytext); return ERROR_DEF;
WARN_[A-Z0-9_]+ yylval.word = strdup(yytext); return WARNING_DEF;
[a-z0-9-]+      yylval.word = strdup(yytext); return NAME;
<INITIAL>\"     BEGIN(STRING);
<STRING>[^\"]*  yylval.word = strdup(yytext); return MESSAGE;
<STRING>\"      BEGIN(INITIAL);
<INITIAL>\n     return NEWLINE;
[ \t]+          /* ignore whitespace */;

%%
