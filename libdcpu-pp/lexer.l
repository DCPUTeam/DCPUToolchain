%{

/**

	File:			lexer.y

	Project:		DCPU-16 Tools
	Component:		LibDCPU-PP

	Authors:		James Rhodes

	Description:	Defines lexer for the preprocessor.

**/
	
#include <lexfix.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <bstrlib.h>
#include "parser.h"

#define HANDLE ECHO

char* pp_trim_and_unescape(char* data)
{
	unsigned int i = 0, a = 0;
	char* result = malloc(strlen(data) - 1);
	memset(result, '\0', strlen(data) - 1);
	for (i = 1; i < strlen(data) - 1; i++)
	{
		if (data[i] == '\\' && data[i+1] == '"')
		{
			result[a] = '"';
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

%option noyywrap
%option yylineno
%option reentrant
%option bison-bridge
%option header-file="lexer.h"
%option prefix="pp_yy"

%x PREPROCESSING

%%

	/* Preprocessor keywords */
	
<INITIAL>[\.\#][Ii][Nn][Cc][Ll][Uu][Dd][Ee]				BEGIN(PREPROCESSING); return INCLUDE;
<INITIAL>[\.\#][Ee][Qq][Uu][Aa][Tt][Ee]					BEGIN(PREPROCESSING); return EQUATE;
<INITIAL>[\.\#][Ee][Qq][Uu]								BEGIN(PREPROCESSING); return EQUATE;
<INITIAL>[\.\#][Dd][Ee][Ff][Ii][Nn][Ee]					BEGIN(PREPROCESSING); return EQUATE;
<INITIAL>[\.\#][Ii][Ff][Dd][Ee][Ff]						BEGIN(PREPROCESSING); return IFDEF;
<INITIAL>[\.\#][Ii][Ff][Nn][Dd][Ee][Ff]					BEGIN(PREPROCESSING); return IFNDEF;
<INITIAL>[\.\#][Ii][Ff]									BEGIN(PREPROCESSING); return IF;
<INITIAL>[\.\#][Ee][Ll][Ss][Ee]							BEGIN(PREPROCESSING); return ELSE;
<INITIAL>[\.\#][Ee][Nn][Dd][Ii][Ff]						BEGIN(PREPROCESSING); return ENDIF;

	/* String and character analysis */

<PREPROCESSING>[A-Za-z_-]+[A-Za-z0-9_-]*				yylval->string = bfromcstr(yytext); return WORD;
<PREPROCESSING>0x[0-9A-Fa-f]+							yylval->number = strtoul(yytext, NULL, 16); return NUMBER;
<PREPROCESSING>[0-9]+									yylval->number = strtoul(yytext, NULL, 10); return NUMBER;
<PREPROCESSING>0[0-7]+									yylval->number = strtoul(yytext, NULL, 8); return NUMBER;
<PREPROCESSING>L?\"(\\.|[^\\"])*\"						yylval->string = bfromcstr(pp_trim_and_unescape((char*)strdup(yytext))); return STRING;

	/* Miscellanous */

<PREPROCESSING>\n										yylval->any = '\n'; BEGIN(INITIAL); return ANY_CHAR;
<PREPROCESSING>[^ \t\n]									yylval->any = yytext[0]; return ANY_CHAR;
<PREPROCESSING>[ \t]+									/* ignore whitespace */;
<INITIAL>\n												yylval->any = '\n'; return ANY_CHAR;
<INITIAL>[^\n]											yylval->any = yytext[0]; return ANY_CHAR;

%%
