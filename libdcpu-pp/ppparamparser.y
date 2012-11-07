%{

#include <assert.h>
#include <ppimpl.h>

void ppparamerror(const char *str);
extern state_t* ppparam_current;
extern list_t* ppparam_output;

%}

// Defines a union for storing all of our data types.
%union
{
	int number;
	bstring string;
	int* token;
	struct expr* expr;
	list_t list;
    parameter_t* parameter;
}

// Define our lexical token names.
%token <token> ADD SUBTRACT MULTIPLY DIVIDE MODULUS EQUALS NOT_EQUALS LESS_THAN LESS_EQUALS GREATER_THAN GREATER_EQUALS
%token <token> PAREN_OPEN PAREN_CLOSE BITWISE_AND BITWISE_BOR BITWISE_XOR BITWISE_NOT BOOLEAN_OR BOOLEAN_AND BINARY_LEFT_SHIFT BINARY_RIGHT_SHIFT
%token <token> EXPRESSION
%token <string> STRING ANGLED_STRING _WORD
%token <number> NUMBER

// Start at the parameters node.
%start parameters

// Define our types.
%type <list> parameters
%type <parameter> parameter
%type <expr> expr

// Set the parser name.
%name-prefix="ppparam"

%%

parameters:
        /* empty */
        {
            /* do nothing */
        } |
        parameters parameter
        {
            /* append parameter */
            list_append(ppparam_output, $2);
            $$ = $1;
        } ;

parameter:
        _WORD
        {
            $$ = malloc(sizeof(parameter_t));
            $$->type = _WORD;
            $$->string = $1;
            assert($1 != NULL);
        } |
        STRING
        {
            $$ = malloc(sizeof(parameter_t));
            $$->type = STRING;
            $$->string = $1;
            assert($1 != NULL);
        } |
        ANGLED_STRING
        {
            $$ = malloc(sizeof(parameter_t));
            $$->type = ANGLED_STRING;
            $$->string = $1;
            assert($1 != NULL);
        } |
        NUMBER
        {
            $$ = malloc(sizeof(parameter_t));
            $$->type = NUMBER;
            $$->number = $1;
        } |
        expr
        {
            $$ = malloc(sizeof(parameter_t));
            $$->type = EXPRESSION;
            $$->expr = $1;
        } ;

expr:
		NUMBER
		{
			$$ = expr_new_number($1);
		} |
		_WORD
		{
			$$ = expr_new_label(bautofree($1));
		} |
		PAREN_OPEN expr PAREN_CLOSE
		{
			$$ = $2;
		} |
		SUBTRACT expr
		{
			$$ = expr_new(expr_new_number(0), EXPR_OP_SUBTRACT, $2);
		} |
		BITWISE_NOT expr
		{
			$$ = expr_new(expr_new_number(0xFFFF), EXPR_OP_XOR, $2);
		} |
		expr DIVIDE expr
		{
			$$ = expr_new($1, EXPR_OP_DIVIDE, $3);
		} |
		expr MULTIPLY expr
		{
			$$ = expr_new($1, EXPR_OP_MULTIPLY, $3);
		} |
		expr SUBTRACT expr
		{
			$$ = expr_new($1, EXPR_OP_SUBTRACT, $3);
		} |
		expr ADD expr
		{
			$$ = expr_new($1, EXPR_OP_ADD, $3);
		} |
		expr MODULUS expr
		{
			$$ = expr_new($1, EXPR_OP_MODULUS, $3);
		} |
		expr BITWISE_AND expr
		{
			$$ = expr_new($1, EXPR_OP_AND, $3);
		} |
		expr BITWISE_XOR expr
		{
			$$ = expr_new($1, EXPR_OP_XOR, $3);
		} |
		expr BITWISE_BOR expr
		{
			$$ = expr_new($1, EXPR_OP_BOR, $3);
		} |
		expr EQUALS expr
		{
			$$ = expr_new($1, EXPR_OP_EQUALS, $3);
		} |
		expr NOT_EQUALS expr
		{
			$$ = expr_new($1, EXPR_OP_NOT_EQUALS, $3);
		} |
		expr LESS_THAN expr
		{
			$$ = expr_new($1, EXPR_OP_LESS_THAN, $3);
		} |
		expr LESS_EQUALS expr
		{
			$$ = expr_new($1, EXPR_OP_LESS_EQUALS, $3);
		} |
		expr GREATER_THAN expr
		{
			$$ = expr_new($1, EXPR_OP_GREATER_THAN, $3);
		} |
		expr GREATER_EQUALS expr
		{
			$$ = expr_new($1, EXPR_OP_GREATER_EQUALS, $3);
		} ;

%%

list_t* ppparam_output;

///
/// Parses the parameters remaining on the current line
/// and returns back a list of parameters.
///
list_t* ppparam_get(state_t* state)
{
    list_t* result = NULL;

    // We have to use a global state due to the way
    // the input function is defined for flex.
    ppparam_current = state;
    ppparam_output = malloc(sizeof(list_t));
    list_init(ppparam_output);
    ppparamparse();
    result = ppparam_output;
    ppparam_current = NULL;
    ppparam_output = NULL;
    return result;
}

///
/// Frees the parameter list.
///
void ppparam_free(list_t* result)
{
    parameter_t* param;

    // Free the contents of the list.
    list_iterator_start(result);
    while (list_iterator_hasnext(result))
    {
        param = list_iterator_next(result);

        // Free parameter data.
        switch (param->type)
        {
        case _WORD:
        case STRING:
        case ANGLED_STRING:
            bdestroy(param->string);
            break;
        }

        // Free parameter.
        free(param);
    }
    list_iterator_stop(result);

    // Free the list itself.
    list_destroy(result);
    free(result);
}

void ppparamerror(const char *str)
{
    // FIXME: Handle error here.
}