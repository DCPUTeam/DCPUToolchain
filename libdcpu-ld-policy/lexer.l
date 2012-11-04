%{

///
/// @addtogroup LibDCPU-LD-Policy
/// @{
///
/// @file
/// @brief Lexer definition for linker policy.
/// @author James Rhodes
/// 
/// Defines the lexer rules for linker policies.
///

#include <lexfix.h>
#include <bstring.h>
#include <stdlib.h>
#include <stdio.h>
#include "policyast.h"
#include "parser.h"

%}

%option yylineno
%option case-insensitive

%x IN_DEFAULTS
%x EXPECT_KEY
%x EXPECT_EQUALS
%x EXPECT_VALUE

%%

    /* Syntax */
    
\#[^\n]*\n                              /* comment */;
\:                                      return COLON;
\(                                      return OPEN_PARAM;
\)                                      return CLOSE_PARAM;
\,                                      return COMMA;
\$[a-z]                                 yylval.string = bfromcstr(yytext); return VARIABLE;
0x[0-9a-fA-F]+                          yylval.number = strtoul(yytext, NULL, 16); return NUMBER;
[0-9]+                                  yylval.number = strtoul(yytext, NULL, 10); return NUMBER;
<*>[ \t]+                               /* ignore whitespace */;
<*>(\r\n|\r|\n)                         /* ignore newlines */;

    /* Policy Types */
    
defaults                                BEGIN(IN_DEFAULTS); return POLICY_SETTINGS;
internal                                return POLICY_INTERNAL;
format                                  return POLICY_FORMAT;

    /* Instruction Types */
    
write                                   return INST_WRITE;
chain                                   return INST_CHAIN;
offset                                  return INST_OFFSET;

    /* Functions */
    
total                                   return FUNC_TOTAL;
field                                   return FUNC_FIELD;
code                                    return FUNC_CODE;
words                                   return FUNC_WORDS;

    /* For loop */
    
for                                     return KEYWORD_FOR;
from                                    return KEYWORD_FROM;
to                                      return KEYWORD_TO;
endfor                                  return KEYWORD_ENDFOR;

    /* Table Types */
    
adjustment                              return TABLE_ADJUSTMENT;
provided                                return TABLE_PROVIDED;
required                                return TABLE_REQUIRED;
section                                 return TABLE_SECTION;
output                                  return TABLE_OUTPUT;
jump                                    return TABLE_JUMP;
optional                                return TABLE_OPTIONAL;

    /* Field Types */
    
label_size                              return FIELD_LABEL_SIZE;
label_text                              return FIELD_LABEL_TEXT;
address                                 return FIELD_ADDRESS;

[a-zA-Z-]+                              yylval.string = bfromcstr(yytext); return WORD;
<IN_DEFAULTS>\{                         BEGIN(EXPECT_KEY);
<EXPECT_KEY>[a-zA-Z0-9_-]+             yylval.string = bfromcstr(yytext); BEGIN(EXPECT_EQUALS); return KEY;
<EXPECT_KEY>\}                          BEGIN(INITIAL);
<EXPECT_EQUALS>\=                       BEGIN(EXPECT_VALUE); return EQUAL;
<EXPECT_VALUE>[^\n]*                    yylval.string = bfromcstr(yytext); BEGIN(EXPECT_KEY); return VALUE;

    /* Default Error Rule */

<*>.|\n                                 {
                                            char* out = malloc(50);
                                            memset(out, '\0', 50);
                                            snprintf(out, 49, "unexpected characters in input '%s' (%i).", yytext, yytext[0]);
                                            yyerror(out);
                                            free(out);
                                            return ERROR;
                                        }

%%

int yywrap()
{
    return 1;
}

///
/// @}
///