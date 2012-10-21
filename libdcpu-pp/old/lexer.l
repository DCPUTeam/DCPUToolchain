%{

/**

	File:           lexer.y

	Project:        DCPU-16 Tools
	Component:      LibDCPU-pp

	Authors:        James Rhodes

	Description:    Defines lexer for the preprocessor.

**/

#include <lexfix.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <bstrlib.h>
#include <simclist.h>
#include "parser.h"

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

char* pp_angle_trim_and_unescape(char* data)
{
	unsigned int i = 0, a = 0;
	char* result = malloc(strlen(data) - 1);
	memset(result, '\0', strlen(data) - 1);
	for (i = 1; i < strlen(data) - 1; i++)
	{
		if (data[i] == '\\' && (data[i+1] == '<' || data[i+1] == '>'))
		{
			result[a] = data[i+1];
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
%option prefix="pp_yy"

%x PREPROCESSING
%x INCPROCESSING
%x MACRODEF
%x MACROCONTENT
%x MACROCALLDEF
%x EQUATEDEF
%x EQUATESPC
%x EQUATEVAL
%x EQUATECMT

%%

	/* Preprocessor keywords */

<INITIAL>^[ \t]*[\.\#][Ll][Ii][Nn][Ee]						BEGIN(PREPROCESSING); return LINE;
<INITIAL>^[ \t]*[\.\#][Uu][Ll][Ii][Nn][Ee]					BEGIN(PREPROCESSING); return ULINE;
<INITIAL>^[ \t]*[\.\#][Ii][Nn][Cc][Ll][Uu][Dd][Ee]				BEGIN(INCPROCESSING); return INCLUDE;
<INITIAL>^[ \t]*[\.\#][Ee][Qq][Uu][Aa][Tt][Ee][ \t]*				BEGIN(EQUATEDEF); return EQUATE;
<INITIAL>^[ \t]*[\.\#][Ee][Qq][Uu][ \t]*					BEGIN(EQUATEDEF); return EQUATE;
<INITIAL>^[ \t]*[\.\#][Dd][Ee][Ff][Ii][Nn][Ee][ \t]*				BEGIN(EQUATEDEF); return EQUATE;
<INITIAL>^[ \t]*[\.\#][Uu][Nn][Dd][Ee][Ff]					BEGIN(PREPROCESSING); return UNDEF;
<INITIAL>^[ \t]*[\.\#][Ii][Ff][Dd][Ee][Ff]					BEGIN(PREPROCESSING); return IFDEF;
<INITIAL>^[ \t]*[\.\#][Ii][Ff][Nn][Dd][Ee][Ff]					BEGIN(PREPROCESSING); return IFNDEF;
<INITIAL>^[ \t]*[\.\#][Ii][Ff]							BEGIN(PREPROCESSING); return IF;
<INITIAL>^[ \t]*[\.\#][Ee][Ll][Ss][Ee]						BEGIN(PREPROCESSING); return ELSE;
<INITIAL>^[ \t]*[\.\#][Ee][Nn][Dd][Ii][Ff]					BEGIN(PREPROCESSING); return ENDIF;
<INITIAL>^[ \t]*[\.\#][Mm][Aa][Cc][Rr][Oo]					BEGIN(MACRODEF); return MACRO;
<INITIAL,MACROCONTENT>^[ \t]*[\.\#][Ee][Nn][Dd][Mm][Aa][Cc][Rr][Oo]		BEGIN(PREPROCESSING); return ENDMACRO;
<INITIAL>^[ \t]*[\.\#][\.\#]							BEGIN(MACROCALLDEF); return MACROCALL;
<INITIAL>^[ \t]*[\.\#][A-Za-z]+							BEGIN(PREPROCESSING); yylval->string = bfromcstr(yytext); btrimws(yylval->string); bdelete(yylval->string, 0, 1); return CUSTOM;

	/* String and character analysis */

<MACRODEF,MACROCALLDEF,PREPROCESSING>[A-Za-z_]+[A-Za-z0-9_]*		yylval->string = bfromcstr(yytext); return WORD;
<MACROCALLDEF,PREPROCESSING>0x[0-9A-Fa-f]+				yylval->number = strtoul(yytext, NULL, 16); return NUMBER;
<MACROCALLDEF,PREPROCESSING>[1-9]+[0-9]*				yylval->number = strtoul(yytext, NULL, 10); return NUMBER;
<MACROCALLDEF,PREPROCESSING>0[0-7]*					yylval->number = strtoul(yytext, NULL, 8); return NUMBER;
<MACROCALLDEF,PREPROCESSING,INCPROCESSING>L?\"(\\.|[^\\"])*\"		yylval->string = bfromcstr(pp_trim_and_unescape((char*)strdup(yytext))); return STRING;
<INCPROCESSING>L?\<(\\.|[^\\"])*\>					yylval->string = bfromcstr(pp_angle_trim_and_unescape((char*)strdup(yytext))); return STRING;

	/* Equate definitions */
	
<EQUATEDEF>[A-Za-z_]+[A-Za-z0-9_]*					yylval->string = bfromcstr(yytext); BEGIN(EQUATESPC); return WORD;
<EQUATESPC>(\r\n|\r|\n)							yylval->any = '\n'; BEGIN(INITIAL); return NEWLINE;
<EQUATESPC>[ \t]+							BEGIN(EQUATEVAL);
<EQUATESPC>;								BEGIN(EQUATECMT);
<EQUATEVAL>(\r\n|\r|\n)							yylval->any = '\n'; BEGIN(INITIAL); return NEWLINE;
<EQUATEVAL>[^\r\n;]							yylval->any = yytext[0]; return TEXT;
<EQUATEVAL>;								BEGIN(EQUATECMT);
<EQUATECMT>(\r\n|\r|\n)							BEGIN(INITIAL); return NEWLINE;
<EQUATECMT>[^\r\n;]							{ }

	/* Macro definitions */

<MACROCALLDEF,MACRODEF>\(						return PARAM_OPEN;
<MACROCALLDEF>\)							return PARAM_CLOSE;
<MACRODEF>\)								BEGIN(MACROCONTENT); return PARAM_CLOSE;
<MACROCALLDEF,MACRODEF>\,						return COMMA;
<MACROCALLDEF,MACRODEF>(\r\n|\r|\n)					yylval->any = '\n'; BEGIN(INITIAL); return NEWLINE;
<MACROCALLDEF,MACRODEF>[^ \t\r\n\(\)\,]					yylval->any = yytext[0]; return WHITESPACE;
<MACROCALLDEF,MACRODEF>[ \t]+						/* ignore whitespace */;

	/* Expression operators */
	
<PREPROCESSING>\(							return PAREN_OPEN;
<PREPROCESSING>\)							return PAREN_CLOSE;
<PREPROCESSING>\+							return ADD;
<PREPROCESSING>\-							return SUBTRACT;
<PREPROCESSING>\*							return MULTIPLY;
<PREPROCESSING>\/							return DIVIDE;
<PREPROCESSING>\%							return MODULUS;
<PREPROCESSING>\==							return EQUALS;
<PREPROCESSING>\!=							return NOT_EQUALS;
<PREPROCESSING>\<							return LESS_THAN;
<PREPROCESSING>\<=							return LESS_EQUALS;
<PREPROCESSING>\>							return GREATER_THAN;
<PREPROCESSING>\>=							return GREATER_EQUALS;
<PREPROCESSING>\&							return BITWISE_AND;
<PREPROCESSING>\|							return BITWISE_BOR;
<PREPROCESSING>\^							return BITWISE_XOR;
<PREPROCESSING>\~							return BITWISE_NOT;

	/* Miscellanous */

<PREPROCESSING,INCPROCESSING>(\r\n|\r|\n)				yylval->any = '\n'; BEGIN(INITIAL); return NEWLINE;
<PREPROCESSING,INCPROCESSING>[^ \t\r\n]					yylval->any = yytext[0]; return WHITESPACE;
<PREPROCESSING,INCPROCESSING>[ \t]+					/* ignore whitespace */;
<MACROCONTENT,INITIAL>(\r\n|\r|\n)					yylval->any = '\n'; return NEWLINE;
<MACROCONTENT,INITIAL>[^\r\n]						yylval->any = yytext[0]; return TEXT;
<<EOF>>									{ static int once = 0; return (once = !once) ? NEWLINE : 0; }

%%
