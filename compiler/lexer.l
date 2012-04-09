%{

/**

	File:			lexer.y

	Project:		DCPU-16 Tools
	Component:		Compiler

	Authors:		James Rhodes

	Description:	Defines lexer for the compiler.

**/

#include <string>
#include <cstdio>
#include "Node.h"
#include "parser.hpp"

unsigned int yylineno = 1;
int yycolumn = 0;
void count();
void comment_line();
void comment();
extern "C" int yywrap() { return 1; }
char* trim_and_unescape(char* data, int mode);

%}

%x IN_CHAR
%x IN_STRING

%%

	/* Statement keywords */

"return"				{ count(); return RETURN; }
"if"					{ count(); return IF; }
"else"					{ count(); return ELSE; }

	/* String and character analysis */

\'												{ ECHO; yylval.string = new std::string(); BEGIN(IN_CHAR); }
\"												{ ECHO; yylval.string = new std::string(); BEGIN(IN_STRING); }
<IN_CHAR>\'										{ ECHO; count(); BEGIN(INITIAL); return CHARACTER; }
<IN_STRING>\"									{ ECHO; count(); BEGIN(INITIAL); return STRING; }
<IN_STRING,IN_CHAR>\\a							{ ECHO; *yylval.string += '\a'; }
<IN_STRING,IN_CHAR>\\b							{ ECHO; *yylval.string += '\b'; }
<IN_STRING,IN_CHAR>\\f							{ ECHO; *yylval.string += '\f'; }
<IN_STRING,IN_CHAR>\\n							{ ECHO; *yylval.string += '\n'; }
<IN_STRING,IN_CHAR>\\r							{ ECHO; *yylval.string += '\r'; }
<IN_STRING,IN_CHAR>\\t							{ ECHO; *yylval.string += '\t'; }
<IN_STRING,IN_CHAR>\\v							{ ECHO; *yylval.string += '\v'; }
<IN_STRING,IN_CHAR>\\\'							{ ECHO; *yylval.string += '\''; }
<IN_STRING,IN_CHAR>\\\"							{ ECHO; *yylval.string += '\"'; }
<IN_STRING,IN_CHAR>\\\\							{ ECHO; *yylval.string += '\\'; }
<IN_STRING,IN_CHAR>\\\?							{ ECHO; *yylval.string += '\?'; }
<IN_STRING,IN_CHAR>\\[0-7]{1,3}					{ ECHO; *yylval.string += strtoul(yytext + 1, NULL, 8); }
<IN_STRING,IN_CHAR>\\x[a-fA-F0-9]{2}			{ ECHO; *yylval.string += strtoul(yytext + 2, NULL, 16); }
<IN_STRING,IN_CHAR>\\[0-9]+						{ /* Generate error: bad escape sequence */ }
<IN_STRING,IN_CHAR>\\(.|\n)						{ ECHO; *yylval.string += yytext[1]; }
<IN_STRING>[^\\\n\"]+							{ ECHO; *yylval.string += yytext; }
<IN_CHAR>[^\\\n\']+								{ ECHO; *yylval.string += yytext; }

	/* Identifiers, numbers and basic lexical components */

[A-Za-z_][A-Za-z0-9_]*	{ yylval.string = new std::string(strdup(yytext)); count(); return IDENTIFIER; }
0x[0-9A-Fa-f]+			{ yylval.number = strtoul(yytext, NULL, 16); count(); return NUMBER; }
[1-9]+[0-9]*			{ yylval.number = strtoul(yytext, NULL, 10); count(); return NUMBER; }
0[0-7]*					{ yylval.number = strtoul(yytext, NULL, 8); count(); return NUMBER; }
0b[0-1]+				{ yylval.number = strtoul(yytext, NULL, 2); count(); return NUMBER; }
"("						{ count(); return yylval.token = CURVED_OPEN; }
")"						{ count(); return yylval.token = CURVED_CLOSE; }
"{"						{ count(); return yylval.token = BRACE_OPEN; }
"}"						{ count(); return yylval.token = BRACE_CLOSE; }
","						{ count(); return yylval.token = COMMA; }
"*"						{ count(); return yylval.token = STAR; }
";"						{ count(); return yylval.token = SEMICOLON; }
"."						{ count(); return yylval.token = DOT; }

	/* Assignment, equivilance and mathematical operators */

"="						{ count(); return yylval.token = ASSIGN_EQUAL; }
"+="					{ count(); return yylval.token = ASSIGN_ADD; }
"-="					{ count(); return yylval.token = ASSIGN_SUBTRACT; }
"*="					{ count(); return yylval.token = ASSIGN_MULTIPLY; }
"/="					{ count(); return yylval.token = ASSIGN_DIVIDE; }
"=="					{ count(); return yylval.token = COMPARE_EQUAL; }
"!="					{ count(); return yylval.token = COMPARE_NOT_EQUAL; }
"<"						{ count(); return yylval.token = COMPARE_LESS_THAN; }
"<="					{ count(); return yylval.token = COMPARE_LESS_THAN_EQUAL; }
">"						{ count(); return yylval.token = COMPARE_GREATER_THAN; }
">="					{ count(); return yylval.token = COMPARE_GREATER_THAN_EQUAL; }
"!"						{ count(); return yylval.token = NEGATE; }
"++"					{ count(); return yylval.token = INCREMENT; }
"--"					{ count(); return yylval.token = DECREMENT; }
"+"						{ count(); return yylval.token = ADD; }
"-"						{ count(); return yylval.token = SUBTRACT; }
"/"						{ count(); return yylval.token = SLASH; }
"&&"					{ count(); return yylval.token = BOOLEAN_AND; }
"||"					{ count(); return yylval.token = BOOLEAN_OR; }
"&"						{ count(); return yylval.token = BINARY_AND; }
"|"						{ count(); return yylval.token = BINARY_OR; }
"^"						{ count(); return yylval.token = BINARY_XOR; }
"<<"					{ count(); return yylval.token = BINARY_LEFT_SHIFT; }
">>"					{ count(); return yylval.token = BINARY_RIGHT_SHIFT; }

	/* Comments and whitespace */

\n						{ yylineno++; }
"//"					{ comment_line(); }
"/*"					{ comment(); }
[ \t]+                  /* ignore whitespace */;

%%

void comment()
{
	char c, c1;

loop:
	while ((c = yyinput()) != '*' && c != 0) ;

	if ((c1 = yyinput()) != '/' && c != 0)
		goto loop;
}

void comment_line()
{
	char c;

	while ((c = yyinput()) != '\n' && c != 0) ;
}

void count()
{
	int i;

	for (i = 0; yytext[i] != '\0'; i++)
		if (yytext[i] == '\n')
			yycolumn = 0;
		else if (yytext[i] == '\t')
			yycolumn += 8 - (yycolumn % 8);
		else
			yycolumn++;

	ECHO;
}