%{

/**

	File:           lexer.y

	Project:        DCPU-16 Tools
	Component:      LibDCPU-vm-dbg

	Authors:        James Rhodes

	Description:    Defines lexer for the debugger's command-line input.

**/

#include <lexfix.h>
#include <bstrlib.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "parser.h"

char* dbg_trim_and_unescape(char* data, int mode);

#define COPY yylval->string = bfromcstr(strdup(yytext));
#define COPY_HEX yylval->number = strtoul(yytext, NULL, 16);
#define COPY_DEC yylval->number = strtoul(yytext, NULL, 10);
#define COPY_UNESCAPE yylval->string = bfromcstr(dbg_trim_and_unescape(strdup(yytext), 1));

%}

%option case-insensitive
%option interactive
%option yylineno
%option noyywrap
%option reentrant
%option bison-bridge
%option prefix="dbg_yy"


%%

	/* Debugger command identifiers */

"load"						COPY; return ID_LOAD;
"symbols"					COPY; return ID_SYMBOLS;
"breakpoint"					COPY; return ID_BREAKPOINT;
"break"						COPY; return ID_BREAKPOINT;
"disassemble"					COPY; return ID_DISASSEMBLE;
"disasm"					COPY; return ID_DISASSEMBLE;
"run"						COPY; return ID_RUN;
"continue"					COPY; return ID_CONTINUE;
"stop"						COPY; return ID_STOP;
"quit"						COPY; return ID_QUIT;
"add"						COPY; return ID_ADD;
"delete"					COPY; return ID_DELETE;
"attach"					COPY; return ID_ATTACH;
"inspect"					COPY; return ID_INSPECT;
"hardware"					COPY; return ID_HARDWARE;
"cpu"						COPY; return ID_CPU;
"detach"					COPY; return ID_DETACH;
"list"						COPY; return ID_LIST;
"memory"					COPY; return ID_MEMORY;
"help"						COPY; return ID_HELP;
"step"						COPY; return ID_STEP;
"set"						COPY; return ID_SET;
"debug"						COPY; return ID_DEBUG;
\?							COPY; return ID_HELP;

	/* General stuff */

0x[0-9A-Fa-f]+					COPY_HEX; return ADDRESS;
[0-9]+							COPY_DEC; return ADDRESS;
[\.\/A-Za-z0-9-]+				COPY; return PATH;
[A-Za-z_-]+[A-Za-z0-9_-]*		COPY; return PARAM;
L?'(\\.|[^\\'])+'				COPY_UNESCAPE return CHARACTER;
L?\"(\\.|[^\\"])*\"				COPY_UNESCAPE; return STRING;
\:								return COLON;

[ \t]+						/* ignore whitespace */;

%%

char* dbg_trim_and_unescape(char* data, int mode)
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
