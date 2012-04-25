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
#include "dcpu.h"

unsigned int yyalineno = 1;

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

%x IN_OPCODE

%%

	/* Assembler keywords */

\.[Bb][Oo][Uu][Nn][Dd][Aa][Rr][Yy]			yylval.number = BOUNDARY; return KEYWORD;
\.[Ee][Xx][Tt][Ee][Nn][Ss][Ii][Oo][Nn]		yylval.number = EXTENSION; return KEYWORD;
\.[Or][Rr][Ii][Gg][Ii][Nn]					yylval.number = ORIGIN; return KEYWORD;
\.[Oo][Rr][Gg]								yylval.number = ORIGIN; return KEYWORD;
\.[Ii][Nn][Cc][Bb][Ii][Nn]					yylval.number = INCBIN; return KEYWORD;
\.[Ee][Xx][Pp][Oo][Rr][Tt]					yylval.number = EXPORT; return KEYWORD;
\.[Ii][Mm][Pp][Oo][Rr][Tt]					yylval.number = IMPORT; return KEYWORD;

	/* Opcodes as numbers support */

\{							BEGIN(IN_OPCODE);
<IN_OPCODE>\}				BEGIN(INITIAL);
<IN_OPCODE>[Ss][Ee][Tt]		yylval.number = OP_SET; return ADDRESS;
<IN_OPCODE>[Aa][Dd][Dd]		yylval.number = OP_ADD; return ADDRESS;
<IN_OPCODE>[Ss][Uu][Bb]		yylval.number = OP_SUB; return ADDRESS;
<IN_OPCODE>[Mm][Uu][Ll]		yylval.number = OP_MUL; return ADDRESS;
<IN_OPCODE>[Mm][Ll][Ii]		yylval.number = OP_MLI; return ADDRESS;
<IN_OPCODE>[Dd][Ii][Vv]		yylval.number = OP_DIV; return ADDRESS;
<IN_OPCODE>[Dd][Vv][Ii]		yylval.number = OP_DVI; return ADDRESS;
<IN_OPCODE>[Mm][Oo][Dd]		yylval.number = OP_MOD; return ADDRESS;
<IN_OPCODE>[Aa][Nn][Dd]		yylval.number = OP_AND; return ADDRESS;
<IN_OPCODE>[Bb][Oo][Rr]		yylval.number = OP_BOR; return ADDRESS;
<IN_OPCODE>[Xx][Oo][Rr]		yylval.number = OP_XOR; return ADDRESS;
<IN_OPCODE>[Ss][Hh][Rr]		yylval.number = OP_SHR; return ADDRESS;
<IN_OPCODE>[Aa][Ss][Rr]		yylval.number = OP_ASR; return ADDRESS;
<IN_OPCODE>[Ss][Hh][Ll]		yylval.number = OP_SHL; return ADDRESS;
<IN_OPCODE>[Mm][Vv][Ii]		yylval.number = OP_MVI; return ADDRESS;
<IN_OPCODE>[Ii][Ff][Bb]		yylval.number = OP_IFB; return ADDRESS;
<IN_OPCODE>[Ii][Ff][Cc]		yylval.number = OP_IFC; return ADDRESS;
<IN_OPCODE>[Ii][Ff][Ee]		yylval.number = OP_IFE; return ADDRESS;
<IN_OPCODE>[Ii][Ff][Nn]		yylval.number = OP_IFN; return ADDRESS;
<IN_OPCODE>[Ii][Ff][Gg]		yylval.number = OP_IFG; return ADDRESS;
<IN_OPCODE>[Ii][Ff][Aa]		yylval.number = OP_IFA; return ADDRESS;
<IN_OPCODE>[Ii][Ff][Ll]		yylval.number = OP_IFL; return ADDRESS;
<IN_OPCODE>[Ii][Ff][Uu]		yylval.number = OP_IFU; return ADDRESS;
<IN_OPCODE>[Aa][Dd][Xx]		yylval.number = OP_ADX; return ADDRESS;
<IN_OPCODE>[Ss][Uu][Xx]		yylval.number = OP_SUX; return ADDRESS;
<IN_OPCODE>[Jj][Ss][Rr]		yylval.number = NBOP_JSR; return ADDRESS;
<IN_OPCODE>[Ii][Nn][Tt]		yylval.number = NBOP_INT; return ADDRESS;
<IN_OPCODE>[Ii][Aa][Gg]		yylval.number = NBOP_IAG; return ADDRESS;
<IN_OPCODE>[Ii][Aa][Ss]		yylval.number = NBOP_IAS; return ADDRESS;
<IN_OPCODE>[Hh][Ww][Nn]		yylval.number = NBOP_HWN; return ADDRESS;
<IN_OPCODE>[Hh][Ww][Qq]		yylval.number = NBOP_HWQ; return ADDRESS;
<IN_OPCODE>[Hh][Ww][Ii]		yylval.number = NBOP_HWI; return ADDRESS;
<IN_OPCODE>[^ \n\t]+		return ERROR;

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

\n							yyalineno++; return NEWLINE;
\;[^\r\n]*					/* ignore comments */;
[ \t]+						/* ignore whitespace */;

%%
