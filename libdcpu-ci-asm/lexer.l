%{

/**

    File:           lexer.y

    Project:        DCPU-16 Tools
    Component:      Assembler

    Authors:        James Rhodes

    Description:    Defines lexer for the assembler.

**/

#include <lexfix.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <bstring.h>
#include <rparse.h>
#include "parser.h"
#include "dcpu.h"

// Filename tracking for language producing assembly.
int yyulineno = 0;
bstring yyufilename = NULL;

// Filename tracking for local assembly.
bstring yyfilename = NULL;

%}

%option yylineno

%x IN_OPCODE
%x IN_REG
%x IN_VAL
%x IN_NXT
%x IN_CHAR
%x IN_STRING
%x IN_SECTION
%x IN_SYMBOL

%%

    /* Line and file directives */
    
\#\ [0-9]+(\r\n|\r|\n)                  rparse_dol(yytext, &yylineno);
\#\ [0-9]+\ [^\r\n]*(\r\n|\r|\n)        rparse_do(yytext, &yylineno, &yyfilename);
\#U\ [0-9]+\ [^\r\n]*(\r\n|\r|\n)       rparse_do(yytext, &yyulineno, &yyufilename);

    /* Assembler keywords */

\.[Bb][Oo][Uu][Nn][Dd][Aa][Rr][Yy]      yylval.number = BOUNDARY; return KEYWORD;
\.[Ee][Xx][Tt][Ee][Nn][Ss][Ii][Oo][Nn]  yylval.number = EXTENSION; return KEYWORD;
\.[Ss][Ee][Ee][Kk]                      yylval.number = SEEK; return KEYWORD;
\.[Oo][Rr][Ii][Gg][Ii][Nn]              yylval.number = ORIGIN; return KEYWORD;
\.[Oo][Rr][Gg]                          yylval.number = ORIGIN; return KEYWORD;
\.[Ii][Nn][Cc][Bb][Ii][Nn]              yylval.number = INCBIN; return KEYWORD;
\.[Ee][Xx][Pp][Oo][Rr][Tt]              yylval.number = EXPORT; return KEYWORD;
\.[Ii][Mm][Pp][Oo][Rr][Tt]              yylval.number = IMPORT; return KEYWORD;
[Oo][Pp][Tt][Ii][Oo][Nn][Aa][Ll]        yylval.number = IMPORT_OPTIONAL; return IMPORT_OPTIONAL;
\.[Ff][Ii][Ll][Ll]                      yylval.number = FILL; return FILL;
\.[Ss][Ee][Cc][Tt][Ii][Oo][Nn]          yylval.number = SECTION; return KEYWORD;
\.[Oo][Uu][Tt][Pp][Uu][Tt]              yylval.number = OUTPUT; return KEYWORD;
\.[Jj][Uu][Mm][Pp]                      yylval.number = JUMP; return KEYWORD;
\.[Cc][Aa][Ll][Ll]                      yylval.number = CALL; return KEYWORD;
\.[Ss][Yy][Mm][Bb][Oo][Ll][ \t]*        BEGIN(IN_SYMBOL); yylval.number = SYMBOL; return KEYWORD;
<IN_SYMBOL>[^\n]+                       yylval.string = bfromcstr(yytext); return STRING;
<IN_SYMBOL>(\r\n|\r|\n)                 BEGIN(INITIAL); return NEWLINE;

    /* Opcodes as numbers support */

\{                          BEGIN(IN_OPCODE);
<IN_OPCODE,IN_REG,IN_VAL,IN_NXT>\} BEGIN(INITIAL);
<IN_OPCODE>[Ss][Ee][Tt]     yylval.number = OP_SET; return ADDRESS;
<IN_OPCODE>[Aa][Dd][Dd]     yylval.number = OP_ADD; return ADDRESS;
<IN_OPCODE>[Ss][Uu][Bb]     yylval.number = OP_SUB; return ADDRESS;
<IN_OPCODE>[Mm][Uu][Ll]     yylval.number = OP_MUL; return ADDRESS;
<IN_OPCODE>[Mm][Ll][Ii]     yylval.number = OP_MLI; return ADDRESS;
<IN_OPCODE>[Dd][Ii][Vv]     yylval.number = OP_DIV; return ADDRESS;
<IN_OPCODE>[Dd][Vv][Ii]     yylval.number = OP_DVI; return ADDRESS;
<IN_OPCODE>[Mm][Oo][Dd]     yylval.number = OP_MOD; return ADDRESS;
<IN_OPCODE>[Mm][Dd][Ii]     yylval.number = OP_MDI; return ADDRESS;
<IN_OPCODE>[Aa][Nn][Dd]     yylval.number = OP_AND; return ADDRESS;
<IN_OPCODE>[Bb][Oo][Rr]     yylval.number = OP_BOR; return ADDRESS;
<IN_OPCODE>[Xx][Oo][Rr]     yylval.number = OP_XOR; return ADDRESS;
<IN_OPCODE>[Ss][Hh][Rr]     yylval.number = OP_SHR; return ADDRESS;
<IN_OPCODE>[Aa][Ss][Rr]     yylval.number = OP_ASR; return ADDRESS;
<IN_OPCODE>[Ss][Hh][Ll]     yylval.number = OP_SHL; return ADDRESS;
<IN_OPCODE>[Ii][Ff][Bb]     yylval.number = OP_IFB; return ADDRESS;
<IN_OPCODE>[Ii][Ff][Cc]     yylval.number = OP_IFC; return ADDRESS;
<IN_OPCODE>[Ii][Ff][Ee]     yylval.number = OP_IFE; return ADDRESS;
<IN_OPCODE>[Ii][Ff][Nn]     yylval.number = OP_IFN; return ADDRESS;
<IN_OPCODE>[Ii][Ff][Gg]     yylval.number = OP_IFG; return ADDRESS;
<IN_OPCODE>[Ii][Ff][Aa]     yylval.number = OP_IFA; return ADDRESS;
<IN_OPCODE>[Ii][Ff][Ll]     yylval.number = OP_IFL; return ADDRESS;
<IN_OPCODE>[Ii][Ff][Uu]     yylval.number = OP_IFU; return ADDRESS;
<IN_OPCODE>[Aa][Dd][Xx]     yylval.number = OP_ADX; return ADDRESS;
<IN_OPCODE>[Ss][Bb][Xx]     yylval.number = OP_SBX; return ADDRESS;
<IN_OPCODE>[Ss][Tt][Ii]     yylval.number = OP_STI; return ADDRESS;
<IN_OPCODE>[Ss][Tt][Dd]     yylval.number = OP_STD; return ADDRESS;
<IN_OPCODE>[Jj][Ss][Rr]     yylval.number = NBOP_JSR; return ADDRESS;
<IN_OPCODE>[Ii][Nn][Tt]     yylval.number = NBOP_INT; return ADDRESS;
<IN_OPCODE>[Ii][Aa][Gg]     yylval.number = NBOP_IAG; return ADDRESS;
<IN_OPCODE>[Ii][Aa][Ss]     yylval.number = NBOP_IAS; return ADDRESS;
<IN_OPCODE>[Rr][Ff][Ii]     yylval.number = NBOP_RFI; return ADDRESS;
<IN_OPCODE>[Ii][Aa][Qq]     yylval.number = NBOP_IAQ; return ADDRESS;
<IN_OPCODE>[Hh][Ww][Nn]     yylval.number = NBOP_HWN; return ADDRESS;
<IN_OPCODE>[Hh][Ww][Qq]     yylval.number = NBOP_HWQ; return ADDRESS;
<IN_OPCODE>[Hh][Ww][Ii]     yylval.number = NBOP_HWI; return ADDRESS;

<IN_OPCODE>[Rr][Ee][Gg]\:   BEGIN(IN_REG);
<IN_REG>[Aa]            yylval.number = REG_A; return ADDRESS;
<IN_REG>[Bb]            yylval.number = REG_B; return ADDRESS;
<IN_REG>[Cc]            yylval.number = REG_C; return ADDRESS;
<IN_REG>[Xx]            yylval.number = REG_X; return ADDRESS;
<IN_REG>[Yy]            yylval.number = REG_Y; return ADDRESS;
<IN_REG>[Zz]            yylval.number = REG_Z; return ADDRESS;
<IN_REG>[Ii]            yylval.number = REG_I; return ADDRESS;
<IN_REG>[Jj]            yylval.number = REG_J; return ADDRESS;

<IN_OPCODE>[Vv][Aa][Ll]\:   BEGIN(IN_VAL);
<IN_VAL>[Aa]            yylval.number = VAL_A; return ADDRESS;
<IN_VAL>[Bb]            yylval.number = VAL_B; return ADDRESS;
<IN_VAL>[Cc]            yylval.number = VAL_C; return ADDRESS;
<IN_VAL>[Xx]            yylval.number = VAL_X; return ADDRESS;
<IN_VAL>[Yy]            yylval.number = VAL_Y; return ADDRESS;
<IN_VAL>[Zz]            yylval.number = VAL_Z; return ADDRESS;
<IN_VAL>[Ii]            yylval.number = VAL_I; return ADDRESS;
<IN_VAL>[Jj]            yylval.number = VAL_J; return ADDRESS;

<IN_OPCODE>[Nn][Xx][Tt]\:   BEGIN(IN_NXT);
<IN_NXT>[Aa]            yylval.number = NXT_VAL_A; return ADDRESS;
<IN_NXT>[Bb]            yylval.number = NXT_VAL_B; return ADDRESS;
<IN_NXT>[Cc]            yylval.number = NXT_VAL_C; return ADDRESS;
<IN_NXT>[Xx]            yylval.number = NXT_VAL_X; return ADDRESS;
<IN_NXT>[Yy]            yylval.number = NXT_VAL_Y; return ADDRESS;
<IN_NXT>[Zz]            yylval.number = NXT_VAL_Z; return ADDRESS;
<IN_NXT>[Ii]            yylval.number = NXT_VAL_I; return ADDRESS;
<IN_NXT>[Jj]            yylval.number = NXT_VAL_J; return ADDRESS;
<IN_NXT>[Ll][Ii][Tt]        yylval.number = NXT_LIT; return ADDRESS;

<IN_OPCODE>[Pp][Oo][Pp]     yylval.number = PUSH_POP; return ADDRESS;
<IN_OPCODE>[Pp][Uu][Ss][Hh] yylval.number = PUSH_POP; return ADDRESS;
<IN_OPCODE>[Pp][Ee][Ee][Kk] yylval.number = PEEK; return ADDRESS;
<IN_OPCODE>[Pp][Ii][Cc][Kk] yylval.number = PICK; return ADDRESS;
<IN_OPCODE>[Ss][Pp]     yylval.number = SP; return ADDRESS;
<IN_OPCODE>[Pp][Cc]     yylval.number = PC; return ADDRESS;
<IN_OPCODE>[Ee][Xx]     yylval.number = EX; return ADDRESS;
<IN_OPCODE>[Ii][Aa]     yylval.number = IA; return ADDRESS;
<IN_OPCODE>[Nn][Xx][Tt]     yylval.number = NXT; return ADDRESS;

    /* Expression operators */
    
\(              return PAREN_OPEN;
\)              return PAREN_CLOSE;
\+              return ADD;
\-              return SUBTRACT;
\*              return MULTIPLY;
\/              return DIVIDE;
\%              return MODULUS;
\==             return EQUALS;
\!=             return NOT_EQUALS;
\<              return LESS_THAN;
\<=             return LESS_EQUALS;
\>              return GREATER_THAN;
\>=             return GREATER_EQUALS;
\&              return BITWISE_AND;
\|              return BITWISE_BOR;
\^              return BITWISE_XOR;
\~              return BITWISE_NOT;

    /* String and character analysis */

\'                  yylval.string = bfromcstr(""); BEGIN(IN_CHAR);
\"                  yylval.string = bfromcstr(""); BEGIN(IN_STRING);
<IN_CHAR>\'             BEGIN(INITIAL); return CHARACTER;
<IN_STRING>\"               BEGIN(INITIAL); return STRING;
<IN_STRING,IN_CHAR>\\a          bconchar(yylval.string, '\a');
<IN_STRING,IN_CHAR>\\b          bconchar(yylval.string, '\b');
<IN_STRING,IN_CHAR>\\f          bconchar(yylval.string, '\f');
<IN_STRING,IN_CHAR>\\n          bconchar(yylval.string, '\n');
<IN_STRING,IN_CHAR>\\r          bconchar(yylval.string, '\r');
<IN_STRING,IN_CHAR>\\t          bconchar(yylval.string, '\t');
<IN_STRING,IN_CHAR>\\v          bconchar(yylval.string, '\v');
<IN_STRING,IN_CHAR>\\\'         bconchar(yylval.string, '\'');
<IN_STRING,IN_CHAR>\\\"         bconchar(yylval.string, '\"');
<IN_STRING,IN_CHAR>\\\\         bconchar(yylval.string, '\\');
<IN_STRING,IN_CHAR>\\\?         bconchar(yylval.string, '\?');
<IN_STRING,IN_CHAR>\\[0-7]{1,3}     bconchar(yylval.string, (char)strtoul(yytext + 1, NULL, 8));
<IN_STRING,IN_CHAR>\\x[a-fA-F0-9]{2}    bconchar(yylval.string, (char)strtoul(yytext + 2, NULL, 16));
<IN_STRING,IN_CHAR>\\[0-9]+     return ERROR;
<IN_STRING,IN_CHAR>\\(.|\n)     bconchar(yylval.string, yytext[1]);
<IN_STRING>[^\\\n\"]+           bcatcstr(yylval.string, yytext);
<IN_CHAR>[^\\\n\']+         bcatcstr(yylval.string, yytext);

    /* General stuff */

[abcxyzijABCXYZIJ]      yylval.word = strdup(yytext); return REGISTER;
[Pp][Oo][Pp]            yylval.word = strdup(yytext); return REGISTER;
[Pp][Uu][Ss][Hh]        yylval.word = strdup(yytext); return REGISTER;
[Pp][Ee][Ee][Kk]        yylval.word = strdup(yytext); return REGISTER;
[Pp][Ii][Cc][Kk]        yylval.word = strdup(yytext); return LEX_PICK;
[Ss][Pp]            yylval.word = strdup(yytext); return REGISTER;
[Pp][Cc]            yylval.word = strdup(yytext); return REGISTER;
[Ee][Xx]            yylval.word = strdup(yytext); return REGISTER;
[Ii][Aa]            yylval.word = strdup(yytext); return REGISTER;
[Nn][Xx][Tt]            yylval.word = strdup(yytext); return REGISTER;
[A-Za-z_]+[A-Za-z0-9_]*  yylval.word = strdup(yytext); return WORD;
0b[0-1]+            yylval.number = strtoul(yytext, NULL, 2); return ADDRESS;
0x[0-9A-Fa-f]+          yylval.number = strtoul(yytext, NULL, 16); return ADDRESS;
[0-9]+              yylval.number = strtoul(yytext, NULL, 10); return ADDRESS;
,                    return COMMA;
\[                   return BRACKET_OPEN;
\]                   return BRACKET_CLOSE;
\:                   return COLON;
[\.][A-Za-z0-9_]+    yylval.word = strdup(yytext); return LOCAL_LABEL;

(\r\n|\r|\n)            return NEWLINE;
<<EOF>>             { static int once = 0; return (once = !once) ? NEWLINE : 0; }
\;[^\r\n]*          /* ignore comments */;
[ \t]+              /* ignore whitespace */;

%%

int yywrap()
{
    return 1;
}
