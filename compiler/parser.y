%{
	
/**

	File:			parser.y

	Project:		DCPU-16 Tools
	Component:		Compiler

	Authors:		James Rhodes

	Description:	Defines parser for the compiler.

**/

#define YYDEBUG 1
#define YYERROR_VERBOSE

#include "Node.h"
NDeclarations* program;

extern int yylex();
extern int yycolumn;

void yyerror(const char* msg)
{
	fflush(stdout);
	printf("\n%*s\n%*s\n", yycolumn, "^", yycolumn, msg);
}

%}

%union {
	NBlock* block;
	NExpression* expr;
	NAssignmentExpression* exprassign;
	NStatement* stmt;
	NIdentifier* ident;
	NInteger* numeric;
	NType* type;
	NDeclarations* decls;
	NFunctionDeclaration* function;
	NVariableDeclaration *variable;
	std::vector<NExpression*> *exprvec;
	std::vector<NDeclaration*> *declvec;
	std::vector<NVariableDeclaration*> *varvec;
	std::string* string;
	const char* data;
	long number;
	int token;
}

/* TOKENS: Identifiers, numbers and basic lexical components */
%token <token> CURVED_OPEN CURVED_CLOSE BRACE_OPEN BRACE_CLOSE COMMA
%token <token> STAR SEMICOLON DOT
%token <number> NUMBER
%token <string> IDENTIFIER CHARACTER STRING

/* TOKENS: Assignment, equivilance and mathematical operators */
%token <token> ASSIGN_EQUAL ASSIGN_ADD ASSIGN_SUBTRACT ASSIGN_MULTIPLY ASSIGN_DIVIDE
%token <token> COMPARE_EQUAL COMPARE_NOT_EQUAL COMPARE_LESS_THAN COMPARE_LESS_THAN_EQUAL
%token <token> COMPARE_GREATER_THAN COMPARE_GREATER_THAN_EQUAL
%token <token> NEGATE BITWISE_NEGATE INCREMENT DECREMENT ADD SUBTRACT SLASH PERCENT
%token <token> BOOLEAN_AND BOOLEAN_OR
%token <token> BINARY_AND BINARY_OR BINARY_XOR BINARY_LEFT_SHIFT BINARY_RIGHT_SHIFT
%token <token> IREF IDEREF IADDROF IUNARYPLUS IUNARYMINUS IPREINC IPREDEC IPOSTINC IPOSTDEC

/* TOKENS: Statement keywords */
%token <token> RETURN IF ELSE

/* TYPES */
%type <type> type
%type <ident> ident
%type <expr> expr numeric string character deref
%type <exprassign> expr_assign
%type <varvec> func_decl_args
%type <exprvec> call_args
%type <decls> program func_list
%type <function> func_decl
%type <variable> var_decl
%type <block> block stmts
%type <stmt> stmt stmt_if stmt_return
%type <token> assignop binaryop

/* OPERATOR PRECEDENCE (LOWEST -> HIGHEST) */
%left IPOSTINC IPOSTDEC DOT
%right IPREINC IPREDEC IUNARYPLUS IUNARYMINUS NEGATE BITWISE_NEGATE IADDROF
%left STAR SLASH PERCENT
%left ADD SUBTRACT
%left BINARY_LEFT_SHIFT BINARY_RIGHT_SHIFT
%left COMPARE_LESS_THAN COMPARE_LESS_THAN_EQUAL COMPARE_GREATER_THAN COMPARE_GREATER_THAN_EQUAL
%left COMPARE_EQUAL COMPARE_NOT_EQUAL
%left BINARY_AND
%left BINARY_XOR
%left BINARY_OR
%left BOOLEAN_AND
%left BOOLEAN_OR
%right ASSIGN_EQUAL ASSIGN_ADD ASSIGN_SUBTRACT ASSIGN_MULTIPLY ASSIGN_DIVIDE
%left COMMA

	//%left COMMA
	//%right ASSIGN_EQUAL ASSIGN_ADD ASSIGN_SUBTRACT ASSIGN_MULTIPLY ASSIGN_DIVIDE
	//%left BOOLEAN_OR
	//%left BOOLEAN_AND
	//%left BINARY_OR
	//%left BINARY_XOR
	//%left BINARY_AND
	//%left COMPARE_EQUAL COMPARE_NOT_EQUAL
	//%left COMPARE_LESS_THAN COMPARE_LESS_THAN_EQUAL COMPARE_GREATER_THAN COMPARE_GREATER_THAN_EQUAL
	//%left BINARY_LEFT_SHIFT BINARY_RIGHT_SHIFT
	//%left ADD SUBTRACT
	//%left STAR SLASH PERCENT
	//%right IPREINC IPREDEC IUNARYPLUS IUNARYMINUS NEGATE BITWISE_NEGATE IDEREF IADDROF
	//%right IDEREFASSIGN
	//%left IPOSTINC IPOSTDEC DOT

/* START POINT */
%start program

%%

program:
		func_list
		{
			program = $1;
			$$ = $1;
		} ;

func_list:
		func_decl
		{
			$$ = new NDeclarations();
			$$->definitions.push_back($<function>1);
		} |
		func_list func_decl
		{
			$1->definitions.push_back($<function>2);
		} ;

func_decl:
		type ident CURVED_OPEN func_decl_args CURVED_CLOSE block
		{
			$$ = new NFunctionDeclaration(*$1, *$2, *$4, *$6);
			//delete $4;
		} ;

func_decl_args:
		/* empty */
		{
			$$ = new VariableList();
		} |
		var_decl
		{
			$$ = new VariableList();
			$$->push_back($<variable>1);
		} |
		func_decl_args COMMA var_decl
		{
			$1->push_back($<variable>3);
		} ;

var_decl:
		type ident
		{
			$$ = new NVariableDeclaration(*$1, *$2);
		} |
		type ident ASSIGN_EQUAL expr
		{
			$$ = new NVariableDeclaration(*$1, *$2, $4);
		};
		
ident:
		IDENTIFIER
		{
			$$ = new NIdentifier(*$1);
			delete $1;
		} ;

type:
		ident
		{
			$$ = new NType($<type>1->name, 0);
			delete $1;
		} |
		ident STAR %prec IREF
		{
			$$ = new NType($<type>1->name, 1);
			delete $1;
		} |
		type STAR %prec IREF
		{
			$$ = new NType($<type>1->name, $<type>1->pointerCount + 1);
			delete $1;
		};

block:
		BRACE_OPEN stmts BRACE_CLOSE
		{
			$$ = $2;
		} |
		BRACE_OPEN BRACE_CLOSE
		{
			$$ = new NBlock();
		} ;

stmts:
		stmt
		{
			$$ = new NBlock();
			$$->statements.push_back($<stmt>1);
		} |
		stmts stmt
		{
			$1->statements.push_back($<stmt>2);
		} ;

stmt:
		var_decl SEMICOLON
		{
			$$ = $1;
		} |
		stmt_if |
		stmt_return |
		expr SEMICOLON
		{
			$$ = new NExpressionStatement(*$1);
		} ;

stmt_if:
		IF CURVED_OPEN expr CURVED_CLOSE stmt ELSE stmt
		{
			NBlock* blk1 = new NBlock();
			NBlock* blk2 = new NBlock();
			blk1->statements.push_back($<stmt>5);
			blk2->statements.push_back($<stmt>7);
			$$ = new NIfStatement(*$3, *blk1, blk2);
		} |
		IF CURVED_OPEN expr CURVED_CLOSE stmt
		{
			NBlock* blk1 = new NBlock();
			blk1->statements.push_back($<stmt>5);
			$$ = new NIfStatement(*$3, *blk1);
		} |
		IF CURVED_OPEN expr CURVED_CLOSE block ELSE block
		{
			$$ = new NIfStatement(*$3, *$5, $7);
		} |
		IF CURVED_OPEN expr CURVED_CLOSE block
		{
			$$ = new NIfStatement(*$3, *$5);
		} ;

stmt_return:
		RETURN expr SEMICOLON
		{
			$$ = new NReturnStatement(*$2);
		} ;

deref:
		STAR ident
		{
			$$ = new NDereferenceOperator(*$<ident>2);
		} |
		STAR numeric
		{
			$$ = new NDereferenceOperator(*$2);
		} |
		STAR CURVED_OPEN expr CURVED_CLOSE
		{
			$$ = new NDereferenceOperator(*$3);
		} |
		deref assignop expr
		{
			if ($1->cType == "expression-dereference") // We can't accept NAssignments as the deref in this case.
				$$ = new NAssignment(new NAssignmentDereference(((NDereferenceOperator*)$1)->expr), $2, *$3);
			else
				throw new CompilerException("Unable to apply dereferencing assignment operation to non-dereference operator based LHS.");
		} ;

expr:
		ident assignop expr
		{
			$$ = new NAssignment(new NAssignmentIdentifier(*$1), $2, *$3);
		} |
		deref
		{
			$$ = $1;
		} |
		ident CURVED_OPEN call_args CURVED_CLOSE
		{
			$$ = new NMethodCall(*$1, *$3);
			delete $3;
		} |
		ident
		{
			$<ident>$ = $1;
		} |
		numeric
		{
			$$ = $1;
		} |
		character
		{
			$$ = $1;
		} |
		string
		{
			$$ = $1;
		} |
		expr binaryop expr
		{
			$$ = new NBinaryOperator(*$1, $2, *$3);
		} |
		BINARY_AND expr_assign %prec IADDROF
		{
			$$ = new NAddressOfOperator(*$2);
		} |
		CURVED_OPEN expr CURVED_CLOSE
		{
			$$ = $2;
		} ;

expr_assign:
		ident
		{
			$$ = new NAssignmentIdentifier(*$1);
		} |
		STAR expr %prec IDEREF
		{
			$$ = new NAssignmentDereference(*$2);
		} ;

numeric:
		NUMBER
		{
			$$ = new NInteger($1);
		} ;

character:
		CHARACTER
		{
			$$ = new NCharacter(*$1);
			delete $1;
		} ;

string:
		STRING
		{
			$$ = new NString(*$1);
			delete $1;
		} ;

call_args:
		/* empty */
		{
			$$ = new ExpressionList();
		} |
		expr
		{
			$$ = new ExpressionList();
			$$->push_back($1);
		} |
		call_args COMMA expr
		{
			$1->push_back($3);
		} ;

assignop:
		ASSIGN_EQUAL |
		ASSIGN_ADD |
		ASSIGN_SUBTRACT |
		ASSIGN_MULTIPLY |
		ASSIGN_DIVIDE ;

binaryop:
		BOOLEAN_AND |
		BOOLEAN_OR |
		BINARY_AND | 
		BINARY_OR | 
		BINARY_XOR |
		BINARY_LEFT_SHIFT |
		BINARY_RIGHT_SHIFT |
		COMPARE_EQUAL |
		COMPARE_NOT_EQUAL |
		COMPARE_LESS_THAN |
		COMPARE_LESS_THAN_EQUAL |
		COMPARE_GREATER_THAN |
		COMPARE_GREATER_THAN_EQUAL |
		ADD |
		SUBTRACT | 
		STAR |
		SLASH ;