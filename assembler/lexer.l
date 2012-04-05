%{

#include <stdlib.h>
#include <stdio.h>
#include "parser.h"

unsigned int yylineno = 1;

%}

%%
	
[A-Za-z]+				yylval.string = strdup(yytext); return WORD;
0x[0-9]+				yylval.number = strtoul(yytext, NULL, 16); return ADDRESS;
[0-9]+					yylval.number = strtoul(yytext, NULL, 10); return ADDRESS;
,						return COMMA;
\[						return BRACKET_OPEN;
\]						return BRACKET_CLOSE;
\:						return COLON;
\+						return ADD;

\n						yylineno++; return NEWLINE;
\;[^\r\n]*              /* ignore comments */;
[ \t]+                  /* ignore whitespace */;
%%