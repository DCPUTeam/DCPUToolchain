%{

/**

	File:           lexer.y

	Project:        DCPU-16 Tools
	Component:      LibDCPU-PP-Expr

	Authors:        James Rhodes

	Description:    Defines lexer for the expression parser.

**/

#include <lexfix.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <bstring.h>
#include "parser.h"

%}

%option noyywrap
%option yylineno
%option reentrant
%option bison-bridge
%option prefix="ppexpr_yy"

%%

	/* Expression operators */
	
\(				return PAREN_OPEN;
\)				return PAREN_CLOSE;
\+				return ADD;
\-				return SUBTRACT;
\*				return MULTIPLY;
\/				return DIVIDE;
\%				return MODULUS;
\==				return EQUALS;
\!=				return NOT_EQUALS;
\<				return LESS_THAN;
\<=				return LESS_EQUALS;
\>				return GREATER_THAN;
\>=				return GREATER_EQUALS;
\&				return BITWISE_AND;
\|				return BITWISE_BOR;
\^				return BITWISE_XOR;
\~				return BITWISE_NOT;
\<\<            return LEFT_SHIFT;
\>\>            return RIGHT_SHIFT;
\|\|            return BOOLEAN_OR;
\&\&            return BOOLEAN_AND;
\^\^            return BOOLEAN_XOR;
\!              return BOOLEAN_NOT;

	/* General stuff */

[A-Za-z_]+[A-Za-z0-9_-]*	yylval->word = strdup(yytext); return WORD;
0b[0-1]+			yylval->number = strtoul(yytext, NULL, 2); return ADDRESS;
0x[0-9A-Fa-f]+			yylval->number = strtoul(yytext, NULL, 16); return ADDRESS;
[0-9]+				yylval->number = strtoul(yytext, NULL, 10); return ADDRESS;
[ \t]+				/* ignore whitespace */;

%%
