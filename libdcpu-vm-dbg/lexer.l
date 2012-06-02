%{

/**

	File:           lexer.y

	Project:        DCPU-16 Tools
	Component:      LibDCPU-vm-dbg

	Authors:        James Rhodes

	Description:    Defines lexer for the debugger's command-line input.

**/

#include <lexfix.h>
#include <simclist.h>
#include <bstring.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "parser.h"

char* dbg_trim_and_unescape(char* data, int mode);

#define COPY yylval->string = bfromcstr(strdup(yytext));
#define COPY_LOWER yylval->string = bfromcstr(strdup(yytext)); btolower(yylval->string);
#define COPY_HEX yylval->number = strtoul(yytext, NULL, 16);
#define COPY_DEC yylval->number = strtoul(yytext, NULL, 10);
#define COPY_UNESCAPE yylval->string = bfromcstr(dbg_trim_and_unescape(strdup(yytext), 1));
#define COMMAND COPY_LOWER; btrimws(yylval->string);

%}

%option case-insensitive
%option interactive
%option yylineno
%option noyywrap
%option reentrant
%option bison-bridge
%option prefix="dbg_yy"

%x IN_PARAMS

%%

	/* Debugger command identifiers */

"load"						COMMAND; return ID_LOAD;
"symbols"					COMMAND; return ID_SYMBOLS;
"breakpoint"					COMMAND; return ID_BREAKPOINT;
"break"						COMMAND; return ID_BREAKPOINT;
"bt"						COMMAND; return ID_BACKTRACE;
"backtrace"					COMMAND; return ID_BACKTRACE;
"disassemble"					COMMAND; return ID_DISASSEMBLE;
"disasm"					COMMAND; return ID_DISASSEMBLE;
"run"						COMMAND; return ID_RUN;
"continue"					COMMAND; return ID_CONTINUE;
"stop"						COMMAND; return ID_STOP;
"quit"						COMMAND; return ID_QUIT;
"add"						COMMAND; return ID_ADD;
"delete"					COMMAND; return ID_DELETE;
"attach"					COMMAND; return ID_ATTACH;
"inspect"					COMMAND; return ID_INSPECT;
"hardware"					COMMAND; return ID_HARDWARE;
"cpu"						COMMAND; return ID_CPU;
"detach"					COMMAND; return ID_DETACH;
"list"						COMMAND; return ID_LIST;
"memory"					COMMAND; return ID_MEMORY;
"help"						COMMAND; return ID_HELP;
"step"						COMMAND; return ID_STEP;
"next"						COMMAND; return ID_NEXT;
"set"						COMMAND; return ID_SET;
"debug"						COMMAND; return ID_DEBUG;
\?						COMMAND; return ID_HELP;
^[ \t]*[a-z-]+					COMMAND; return CUSTOM;

	/* General stuff */

0x[0-9A-Fa-f]+			COPY_HEX; return ADDRESS;
[0-9]+				COPY_DEC; return ADDRESS;
[\.\/A-Za-z0-9-]+		COPY; return PATH;
[A-Za-z_-]+[A-Za-z0-9_-]*	COPY; return PARAM;
L?'(\\.|[^\\'])+'		COPY_UNESCAPE return CHARACTER;
L?\"(\\.|[^\\"])*\"		COPY_UNESCAPE; return STRING;
\:				return COLON;
[ \t]+				/* ignore whitespace */;

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
