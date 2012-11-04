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

#include <assert.h>
#include <bstring.h>
#include <stdio.h>
#include "policyast.h"

extern policies_t* active_ast;
extern int yylex();
void yyerror(const char *str);

%}

// Defines a union for storing all of our data types.
%union
{
    int number;
    int* token;
    bstring string;
    list_t* list;
    policy_t* policy;
    policy_instruction_t* instruction;
    policy_value_t* value;
    policy_function_call_t* function;
}

// Define  our lexical token names.
%token <token> COLON OPEN_PARAM CLOSE_PARAM NEWLINE COMMA
%token <token> POLICY_SETTINGS POLICY_INTERNAL POLICY_FORMAT
%token <token> INST_WRITE INST_CHAIN INST_OFFSET
%token <token> FUNC_TOTAL FUNC_FIELD FUNC_CODE FUNC_WORDS
%token <token> KEYWORD_FOR KEYWORD_FROM KEYWORD_TO KEYWORD_ENDFOR
%token <token> TABLE TABLE_ADJUSTMENT TABLE_PROVIDED TABLE_REQUIRED TABLE_SECTION TABLE_OUTPUT TABLE_JUMP TABLE_OPTIONAL
%token <token> FIELD FIELD_LABEL_SIZE FIELD_LABEL_TEXT FIELD_ADDRESS
%token <token> EQUAL FUNCTION LIST ERROR
%token <number> NUMBER
%token <string> KEY VALUE VARIABLE WORD

// Define types.
%type <number> table_opt field_opt
%type <policy> settings
%type <list> instructions
%type <instruction> instruction
%type <value> value words table field
%type <function> function

// Start at the root node.
%start root

%%

root:
        /* empty */
        {
            // active_ast is already initialized.
        } | 
        root policy
        {
            // sections are added in their own handlers.
        } ;
        
settings:
        /* empty */
        {
            // Initialize the settings policy.
            $$ = malloc(sizeof(policy_t));
            $$->type = POLICY_SETTINGS;
            $$->name = NULL;
            $$->instructions = NULL;
            $$->settings = malloc(sizeof(list_t));
            list_init($$->settings);
        } |
        settings KEY EQUAL VALUE
        {
            // Create the new policy setting.
            policy_setting_t* setting = malloc(sizeof(policy_setting_t));
            setting->key = $2;
            setting->value = $4;
            btrimws(setting->key);
            btrimws(setting->value);
            list_append($1->settings, setting);
            $$ = $1;
        } ;
        
policy:
        POLICY_SETTINGS settings
        {
            active_ast->settings = $2;
        } |
        POLICY_INTERNAL OPEN_PARAM WORD CLOSE_PARAM COLON instructions
        {
            policy_t* policy = malloc(sizeof(policy_t));
            policy->type = POLICY_INTERNAL;
            policy->name = $3;
            policy->instructions = $6;
            policy->settings = NULL;
            list_append(&active_ast->policies, policy);
        } |
        POLICY_FORMAT OPEN_PARAM WORD CLOSE_PARAM COLON instructions
        {
            policy_t* policy = malloc(sizeof(policy_t));
            policy->type = POLICY_FORMAT;
            policy->name = $3;
            policy->instructions = $6;
            policy->settings = NULL;
            list_append(&active_ast->policies, policy);
        } ;

instructions:
        /* empty; no instructions */
        {
            $$ = malloc(sizeof(list_t));
            list_init($$);
        } |
        instructions instruction
        {
            list_append($1, $2);
            $$ = $1;
        } ;
        
value:
        NUMBER
        {
            $$ = malloc(sizeof(policy_value_t));
            $$->type = NUMBER;
            $$->runtime = false;
            $$->number = $1;
        } |
        VARIABLE
        {
            $$ = malloc(sizeof(policy_value_t));
            $$->type = VARIABLE;
            $$->runtime = false;
            $$->string = $1;
        } |
        function
        {
            $$ = malloc(sizeof(policy_value_t));
            $$->type = FUNCTION;
            $$->runtime = false;
            $$->function = $1;
        } ;

instruction:
        INST_WRITE function
        {
            $$ = malloc(sizeof(policy_instruction_t));
            $$->type = INST_WRITE;
            $$->value = malloc(sizeof(policy_value_t));
            $$->value->type = FUNCTION;
            $$->value->runtime = false;
            $$->value->function = $2;
            $$->for_variable = NULL;
            $$->for_start = NULL;
            $$->for_end = NULL;
            $$->for_instructions = NULL;
        } |
        INST_CHAIN WORD
        {
            $$ = malloc(sizeof(policy_instruction_t));
            $$->type = INST_CHAIN;
            $$->value = malloc(sizeof(policy_value_t));
            $$->value->type = WORD;
            $$->value->runtime = false;
            $$->value->string = $2;
            $$->for_variable = NULL;
            $$->for_start = NULL;
            $$->for_end = NULL;
            $$->for_instructions = NULL;
        } |
        INST_OFFSET value
        {
            $$ = malloc(sizeof(policy_instruction_t));
            $$->type = INST_OFFSET;
            $$->value = $2;
            $$->for_variable = NULL;
            $$->for_start = NULL;
            $$->for_end = NULL;
            $$->for_instructions = NULL;
        } |
        KEYWORD_FOR VARIABLE KEYWORD_FROM value KEYWORD_TO value instructions KEYWORD_ENDFOR
        {
            $$ = malloc(sizeof(policy_instruction_t));
            $$->type = KEYWORD_FOR;
            $$->value = NULL;
            $$->for_variable = $2;
            $$->for_start = $4;
            $$->for_end = $6;
            $$->for_instructions = $7;
        } ;

table_opt:
        TABLE_ADJUSTMENT { $$ = TABLE_ADJUSTMENT; } |
        TABLE_PROVIDED { $$ = TABLE_PROVIDED; } | 
        TABLE_REQUIRED { $$ = TABLE_REQUIRED; } |
        TABLE_SECTION { $$ = TABLE_SECTION; } | 
        TABLE_OUTPUT { $$ = TABLE_OUTPUT; } |
        TABLE_JUMP { $$ = TABLE_JUMP; } | 
        TABLE_OPTIONAL { $$ = TABLE_OPTIONAL; } ;
        
field_opt:
        FIELD_LABEL_SIZE { $$ = FIELD_LABEL_SIZE; } |
        FIELD_LABEL_TEXT { $$ = FIELD_LABEL_TEXT; } |
        FIELD_ADDRESS { $$ = FIELD_ADDRESS; } ;

table:
        table_opt
        {
            $$ = malloc(sizeof(policy_value_t));
            $$->type = TABLE;
            $$->runtime = false;
            $$->table = $1;
        } ;

field:
        field_opt
        {
            $$ = malloc(sizeof(policy_value_t));
            $$->type = FIELD;
            $$->runtime = false;
            $$->field = $1;
        } ;
        
words:
        value
        {
            $$ = malloc(sizeof(policy_value_t));
            $$->type = LIST;
            $$->runtime = false;
            $$->list = malloc(sizeof(list_t));
            list_init($$->list);
            list_append($$->list, $1);
        } | 
        words COMMA value
        {
            list_append($1->list, $3);
            $$ = $1;
        } ;
        
function:
        FUNC_TOTAL OPEN_PARAM table CLOSE_PARAM
        {
            $$ = malloc(sizeof(policy_function_call_t));
            $$->function = FUNC_TOTAL;
            $$->parameters = malloc(sizeof(list_t));
            list_init($$->parameters);
            list_append($$->parameters, $3);
        } |
        FUNC_FIELD OPEN_PARAM table COMMA value COMMA field CLOSE_PARAM
        {
            $$ = malloc(sizeof(policy_function_call_t));
            $$->function = FUNC_FIELD;
            $$->parameters = malloc(sizeof(list_t));
            list_init($$->parameters);
            list_append($$->parameters, $3);
            list_append($$->parameters, $5);
            list_append($$->parameters, $7);
        } |
        FUNC_WORDS OPEN_PARAM words CLOSE_PARAM
        {
            $$ = malloc(sizeof(policy_function_call_t));
            $$->function = FUNC_WORDS;
            $$->parameters = malloc(sizeof(list_t));
            list_init($$->parameters);
            assert($3->type == LIST);
            list_iterator_start($3->list);
            while (list_iterator_hasnext($3->list))
                list_append($$->parameters, list_iterator_next($3->list));
            list_iterator_stop($3->list);
        } |
        FUNC_CODE
        {
            $$ = malloc(sizeof(policy_function_call_t));
            $$->function = FUNC_CODE;
            $$->parameters = NULL;
        } ;

%%

void yyerror(const char *str)
{
    printf("%s\n", str);
}