%{

///
/// @addtogroup LibDCPU-LD-Policy
/// @{
///
/// @file
/// @brief Lexer definition for linker policy.
/// @author James Rhodes
/// 
/// Defines the parser rules for linker policies.
///

#define YYERROR_VERBOSE

#include <bstring.h>
#include <stdio.h>

extern int yylex();
void yyerror(const char *str);

%}

// Defines a union for storing all of our data types.
%union
{
    int number;
    bstring string;
    int* token;
}

// Define our lexical token names.
%token <token> COLON OPEN_PARAM CLOSE_PARAM NEWLINE COMMA
%token <token> SECT_DEFAULTS SECT_INTERNAL SECT_FORMAT
%token <token> INST_WRITE INST_CHAIN INST_OFFSET
%token <token> FUNC_TOTAL FUNC_FIELD FUNC_CODE FUNC_WORDS
%token <token> KEYWORD_FOR KEYWORD_FROM KEYWORD_TO KEYWORD_ENDFOR
%token <token> TABLE_ADJUSTMENT TABLE_PROVIDED TABLE_REQUIRED TABLE_SECTION TABLE_OUTPUT TABLE_JUMP TABLE_OPTIONAL
%token <token> FIELD_LABEL_SIZE FIELD_LABEL_TEXT FIELD_ADDRESS
%token <token> EQUAL 
%token <number> NUMBER
%token <string> KEY VALUE VARIABLE WORD

// Define types.
%type <token> table field

// Start at the root node.
%start root

%%

root:
        /* empty */
        {
            
        } | 
        root section
        {
            printf("end section.\n");
        } ;

section_name:
        SECT_INTERNAL OPEN_PARAM WORD CLOSE_PARAM
        {
            printf("internal section ( %s )\n", $3->data);
        } |
        SECT_FORMAT OPEN_PARAM WORD CLOSE_PARAM
        {
            printf("format section ( %s )\n", $3->data);
        } ;
        
settings:
        /* empty */
        {
        } |
        settings KEY EQUAL VALUE
        {
            printf("%s = %s\n", $2->data, $4->data);
        } ;
        
section:
        SECT_DEFAULTS settings
        {
            printf("end settings section.\n");
        } |
        section_name COLON instructions
        {
            printf("end section with instructions.\n");
        } ;

instructions:
        /* empty; no instructions */
        {
        } |
        instructions instruction
        {
            printf("end instruction.\n");
        } ;
        
value:
        NUMBER
        {
            printf("number = %i.\n", $1);
        } |
        VARIABLE
        {
            printf("variable = %s.\n", $1->data);
        } |
        function
        {
            printf("function as value.");
        } ;

instruction:
        INST_WRITE function
        {
            printf("end write call.\n");
        } |
        INST_CHAIN WORD
        {
            printf("end chain call.\n");
        } |
        INST_OFFSET value
        {
            printf("end offset call.\n");
        } |
        KEYWORD_FOR VARIABLE KEYWORD_FROM value KEYWORD_TO value instructions KEYWORD_ENDFOR
        {
            printf("for loop.\n");
        } ;
        
        /* VARIABLE KEYWORD_FROM value KEYWORD_TO value instructions KEYWORD_ENDFOR */

table:
        TABLE_ADJUSTMENT |
        TABLE_PROVIDED | 
        TABLE_REQUIRED |
        TABLE_SECTION | 
        TABLE_OUTPUT |
        TABLE_JUMP | 
        TABLE_OPTIONAL ;
        
field:
        FIELD_LABEL_SIZE |
        FIELD_LABEL_TEXT |
        FIELD_ADDRESS ;
        
words:
        value
        {
            printf("word. ");
        } | 
        words COMMA value
        {
            printf("word. ");
        } ;
        
function:
        FUNC_TOTAL OPEN_PARAM table CLOSE_PARAM
        {
            printf("total call ( %i ).\n", (int)$3);
        } |
        FUNC_FIELD OPEN_PARAM table COMMA value COMMA field CLOSE_PARAM
        {
            printf("field call ( %i , ... , %i ).\n", (int)$3, (int)$7);
        } |
        FUNC_WORDS OPEN_PARAM words CLOSE_PARAM
        {
            printf("words call.\n");
        } |
        FUNC_CODE
        {
            printf("code call.\n");
        } ;

%%

void yyerror(const char *str)
{
    printf("%s\n", str);
}