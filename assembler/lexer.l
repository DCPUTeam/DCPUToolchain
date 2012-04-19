%{

/**

	File:			lexer.y

	Project:		DCPU-16 Tools
	Component:		Assembler

	Authors:		James Rhodes

	Description:	Defines lexer for the assembler.

**/
	
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "parser.h"

unsigned int yylineno = 1;

char* trim_and_unescape(char* data, int mode)
{
	unsigned int i = 0, a = 0;
	char* result = malloc(strlen(data) - 1);
	memset(result, '\0', strlen(data) - 1);
	for (i = 1; i < strlen(data) - 1; i++)
	{
		if (data[i] == '\\' && data[i+1] == '"' && mode == 0)
		{
			result[a] = '"';
			i++; // skip '"'
			a++; // increment position
		}
		else if (data[i] == '\\' && data[i+1] == '\'' && mode == 1)
		{
			result[a] = '\'';
			i++; // skip '"'
			a++; // increment position
		}
		else
		{
			result[a] = data[i];
			a++; // increment position
		}
	}
	free(data);
	return result;
}

%}

%%

	/* Assembler keywords */

\.[Bb][Oo][Uu][Nn][Dd][Aa][Rr][Yy]			yylval.number = BOUNDARY; return KEYWORD;
\.[Ee][Xx][Tt][Ee][Nn][Ss][Ii][Oo][Nn]		yylval.number = EXTENSION; return KEYWORD;
\.[Or][Rr][Ii][Gg][Ii][Nn]					yylval.number = ORIGIN; return KEYWORD;
\.[Oo][Rr][Gg]								yylval.number = ORIGIN; return KEYWORD;
\.[Ii][Nn][Cc][Bb][Ii][Nn]					yylval.number = INCBIN; return KEYWORD;
\.[Ee][Xx][Pp][Oo][Rr][Tt]					yylval.number = EXPORT; return KEYWORD;
\.[Ii][Mm][Pp][Oo][Rr][Tt]					yylval.number = IMPORT; return KEYWORD;

	/* General stuff */

[A-Za-z_-]+[A-Za-z0-9_-]*	yylval.string = strdup(yytext); return WORD;
0x[0-9A-Fa-f]+				yylval.number = strtoul(yytext, NULL, 16); return ADDRESS;
[0-9]+						yylval.number = strtoul(yytext, NULL, 10); return ADDRESS;
L?'(\\.|[^\\'])+'			{ yylval.string = trim_and_unescape(strdup(yytext), 1); return CHARACTER; }
L?\"(\\.|[^\\"])*\"			{ yylval.string = trim_and_unescape(strdup(yytext), 0); return STRING; }
,							return COMMA;
\[							return BRACKET_OPEN;
\]							return BRACKET_CLOSE;
\:							return COLON;
\+							return ADD;

\n							yylineno++; return NEWLINE;
\;[^\r\n]*					/* ignore comments */;
[ \t]+						/* ignore whitespace */;

%%