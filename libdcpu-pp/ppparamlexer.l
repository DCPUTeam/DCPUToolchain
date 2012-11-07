%{

#include <assert.h>
#include <ppimpl.h>
#include <stdlib.h>
#include <stdio.h>
#include "ppparamparser.h"

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

extern state_t* ppparam_current;

#define YY_INPUT(buf,result,max_size) \
{ \
    int c = ppimpl_get_input(ppparam_current); \
    result = (c == EOF) ? YY_NULL : (buf[0] = c, 1); \
}

%}

%option noyywrap
%option yylineno
%option prefix="ppparam"

%%

    /* String and character analysis */

[A-Za-z_]+[A-Za-z0-9_]*         ppparamlval.string = bfromcstr(yytext); assert(ppparamlval.string != NULL); return _WORD;
0x[0-9A-Fa-f]+                  ppparamlval.number = strtoul(yytext, NULL, 16); return NUMBER;
[1-9]+[0-9]*                    ppparamlval.number = strtoul(yytext, NULL, 10); return NUMBER;
0[0-7]*                         ppparamlval.number = strtoul(yytext, NULL, 8); return NUMBER;
L?\"(\\.|[^\\"])*\"             ppparamlval.string = bfromcstr(pp_trim_and_unescape((char*)strdup(yytext))); assert(ppparamlval.string != NULL); return STRING;
L?\<(\\.|[^\\\>])*\>             ppparamlval.string = bfromcstr(pp_angle_trim_and_unescape((char*)strdup(yytext))); assert(ppparamlval.string != NULL); return ANGLED_STRING;

    /* Expression operators */
	
\(                              return PAREN_OPEN;
\)                              return PAREN_CLOSE;
\+                              return ADD;
\-                              return SUBTRACT;
\*                              return MULTIPLY;
\/                              return DIVIDE;
\%                              return MODULUS;
\==                             return EQUALS;
\!=                             return NOT_EQUALS;
\<                              return LESS_THAN;
\<=                             return LESS_EQUALS;
>\>                             return GREATER_THAN;
\>=                             return GREATER_EQUALS;
\&                              return BITWISE_AND;
\|                              return BITWISE_BOR;
\^                              return BITWISE_XOR;
\~                              return BITWISE_NOT;
\|\|                            return BOOLEAN_OR;
\&\&                            return BOOLEAN_AND;
\<\<                            return BINARY_LEFT_SHIFT;
\>\>                            return BINARY_RIGHT_SHIFT;

    /* Miscellanous */

(\r\n|\r|\n)                    yyterminate();
[ \t]+                          /* ignore whitespace */;

%%

state_t* ppparam_current;
