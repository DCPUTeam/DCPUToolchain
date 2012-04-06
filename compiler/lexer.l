%{

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

%}

%%

	/* Statement keywords */

"return"				{ count(); return RETURN; }
"if"					{ count(); return IF; }
"else"					{ count(); return ELSE; }

	/* Identifiers, numbers and basic lexical components */

[A-Za-z_][A-Za-z0-9_]*	{ yylval.string = new std::string(strdup(yytext)); count(); return IDENTIFIER; }
0x[0-9A-Fa-f]+			{ yylval.number = strtoul(yytext, NULL, 16); count(); return NUMBER; }
[1-9]+[0-9]*			{ yylval.number = strtoul(yytext, NULL, 10); count(); return NUMBER; }
0[0-7]*					{ yylval.number = strtoul(yytext, NULL, 8); count(); return NUMBER; }
0b[0-1]+				{ yylval.number = strtoul(yytext, NULL, 2); count(); return NUMBER; }
L?'(\\.|[^\\'])+'		{ yylval.string = new std::string(strdup(yytext)); count(); return CHARACTER; }
L?\"(\\.|[^\\"])*\"		{ yylval.string = new std::string(strdup(yytext)); count(); return STRING; }
"("						{ count(); return CURVED_OPEN; }
")"						{ count(); return CURVED_CLOSE; }
"{"						{ count(); return BRACE_OPEN; }
"}"						{ count(); return BRACE_CLOSE; }
","						{ count(); return COMMA; }
"*"						{ count(); return STAR; }
";"						{ count(); return SEMICOLON; }
"."						{ count(); return DOT; }

	/* Assignment, equivilance and mathematical operators */

"="						{ count(); return ASSIGN_EQUAL; }
"+="					{ count(); return ASSIGN_ADD; }
"-="					{ count(); return ASSIGN_SUBTRACT; }
"*="					{ count(); return ASSIGN_MULTIPLY; }
"/="					{ count(); return ASSIGN_DIVIDE; }
"=="					{ count(); return COMPARE_EQUAL; }
"!="					{ count(); return COMPARE_NOT_EQUAL; }
"<"						{ count(); return COMPARE_LESS_THAN; }
"<="					{ count(); return COMPARE_LESS_THAN_EQUAL; }
">"						{ count(); return COMPARE_GREATER_THAN; }
">="					{ count(); return COMPARE_GREATER_THAN_EQUAL; }
"!"						{ count(); return NEGATE; }
"++"					{ count(); return INCREMENT; }
"--"					{ count(); return DECREMENT; }
"+"						{ count(); return ADD; }
"-"						{ count(); return SUBTRACT; }
"/"						{ count(); return SLASH; }
"&&"					{ count(); return BOOLEAN_AND; }
"||"					{ count(); return BOOLEAN_OR; }
"&"						{ count(); return BINARY_AND; }
"|"						{ count(); return BINARY_OR; }
"^"						{ count(); return BINARY_XOR; }
"<<"					{ count(); return BINARY_LEFT_SHIFT; }
">>"					{ count(); return BINARY_RIGHT_SHIFT; }

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