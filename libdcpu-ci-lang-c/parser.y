%{
	
/**

	File:			parser.y

	Project:		DCPU-16 Tools
	Component:		Compiler

	Authors:		James Rhodes
					Michael Gerhaeuser

	Description:	Defines parser for the compiler.

**/

class NAssignmentExpression;
class NInteger;

#include <lexfix.h>
#include <cstdio>
#include <vector>
#include <AsmGenerator.h>
#include <CompilerException.h>
#include "nodes/NBlock.h"
#include "nodes/NDeclarations.h"
#include "nodes/NFunctionDeclaration.h"
#include "nodes/NStructureDeclaration.h"
#include "nodes/NExpressionStatement.h"
#include "nodes/NIfStatement.h"
#include "nodes/NReturnStatement.h"
#include "nodes/NDebugStatement.h"
#include "nodes/NWhileStatement.h"
#include "nodes/NForStatement.h"
#include "nodes/NAssemblyStatement.h"
#include "nodes/NStructureResolutionOperator.h"
#include "nodes/NAssignment.h"
#include "nodes/NDereferenceOperator.h"
#include "nodes/NMethodCall.h"
#include "nodes/NBinaryOperator.h"
#include "nodes/NAddressOfOperator.h"
#include "nodes/NInteger.h"
#include "nodes/NSizeOfOperator.h"
#include "nodes/NCharacter.h"
#include "nodes/NString.h"

// Turn on verbose error messages.
#define YYERROR_VERBOSE

// Defines the root node.
NDeclarations* program;

// YY-stuff.
extern int yylex();
extern int yycolumn;

void yyerror(const char* msg)
{
	fflush(stdout);
	fprintf(stderr, "\n%*s\n%*s\n", yycolumn, "^", yycolumn, msg);
}

%}

%union {
	NBlock* block;
	NExpression* expr;
	NStatement* stmt;
	NIdentifier* ident;
	NInteger* numeric;
	NType* type;
	NDeclarations* decls;
	NFunctionDeclaration* function;
	NStructureDeclaration* structure;
	NVariableDeclaration *variable;
	std::vector<NExpression*> *exprvec;
	std::vector<NDeclaration*> *declvec;
	std::vector<NVariableDeclaration*> *varvec;
	std::string* string;
	const char* data;
	long number;
	int token;
}

/* TOKENS: Error token for Flex to throw an error */
%token <token> ERROR

/* TOKENS: Identifiers, numbers and basic lexical components */
%token <token> CURVED_OPEN CURVED_CLOSE BRACE_OPEN BRACE_CLOSE COMMA
%token <token> STAR SEMICOLON DOT STRUCT
%token <number> NUMBER
%token <string> IDENTIFIER CHARACTER STRING ASSEMBLY

/* TOKENS: Assignment, equivilance and mathematical operators */
%token <token> ASSIGN_EQUAL ASSIGN_ADD ASSIGN_SUBTRACT ASSIGN_MULTIPLY ASSIGN_DIVIDE
%token <token> COMPARE_EQUAL COMPARE_NOT_EQUAL COMPARE_LESS_THAN COMPARE_LESS_THAN_EQUAL
%token <token> COMPARE_GREATER_THAN COMPARE_GREATER_THAN_EQUAL
%token <token> NEGATE BITWISE_NEGATE INCREMENT DECREMENT ADD SUBTRACT SLASH PERCENT
%token <token> BOOLEAN_AND BOOLEAN_OR
%token <token> BINARY_AND BINARY_OR BINARY_XOR BINARY_LEFT_SHIFT BINARY_RIGHT_SHIFT
%token <token> IREF IDEREF IADDROF IUNARYPLUS IUNARYMINUS IPREINC IPREDEC IPOSTINC IPOSTDEC

/* TOKENS: Constants */
%token <token> TRUE FALSE

/* TOKENS: Statement keywords */
%token <token> RETURN IF ELSE WHILE FOR DEBUG SIZEOF

/* TOKENS: Type keywords */
%token <token> TYPE_VOID TYPE_CHAR TYPE_BYTE TYPE_INT TYPE_LONG TYPE_INT8_T
%token <token> TYPE_INT16_T TYPE_INT32_T TYPE_INT64_T TYPE_UINT8_T TYPE_UINT16_T
%token <token> TYPE_UINT32_T TYPE_UINT64_T

/* TYPES */
%type <type> type
%type <ident> ident type_base
%type <expr> expr numeric string character deref fldref addressable
%type <varvec> func_decl_args struct_decl_args
%type <exprvec> call_args
%type <decls> program prog_decl
%type <function> func_decl
%type <structure> struct_decl
%type <variable> var_decl
%type <block> block stmts block_or_stmt
%type <stmt> stmt stmt_if stmt_return stmt_while stmt_for stmt_debug stmt_asm
%type <token> assignop binaryop

/* OPERATOR PRECEDENCE (LOWEST -> HIGHEST) */
%left IPOSTINC IPOSTDEC DOT DEREFDOT
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

/* START POINT */
%start program

%%

program:
		prog_decl
		{
			program = $1;
			$$ = $1;
		} ;

prog_decl:
		func_decl
		{
			$$ = new NDeclarations();
			$$->definitions.push_back($<function>1);
		} |
		struct_decl
		{
			$$ = new NDeclarations();
			$$->definitions.push_back($<structure>1);
		} |
		var_decl SEMICOLON
		{
			$$ = new NDeclarations();
			$$->definitions.push_back($<variable>1);
		} |
		prog_decl func_decl
		{
			$1->definitions.push_back($<function>2);
		} |
		prog_decl struct_decl
		{
			$1->definitions.push_back($<structure>2);
		} |
		prog_decl var_decl SEMICOLON
		{
			$1->definitions.push_back($<variable>2);
		} ;

func_decl:
		type ident CURVED_OPEN func_decl_args CURVED_CLOSE block
		{
			$$ = new NFunctionDeclaration(*$1, *$2, *$4, $6);
			//delete $4;
		} |
		type ident CURVED_OPEN func_decl_args CURVED_CLOSE SEMICOLON
		{
			$$ = new NFunctionDeclaration(*$1, *$2, *$4, NULL);
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

struct_decl:
		STRUCT ident BRACE_OPEN struct_decl_args SEMICOLON BRACE_CLOSE SEMICOLON
		{
			$$ = new NStructureDeclaration(*$2, *$4);
			//delete $4;
		} |
		STRUCT ident BRACE_OPEN BRACE_CLOSE SEMICOLON
		{
			$$ = new NStructureDeclaration(*$2, *(new VariableList()));
			//delete $4;
		} ;

struct_decl_args:
		/* empty */
		{
			$$ = new VariableList();
		} |
		var_decl
		{
			$$ = new VariableList();
			$$->push_back($<variable>1);
		} |
		struct_decl_args SEMICOLON var_decl
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
		} |
		type CURVED_OPEN STAR ident CURVED_CLOSE CURVED_OPEN func_decl_args CURVED_CLOSE
		{
			$$ = new NVariableDeclaration(*(new NFunctionPointerType(*$1, *$7)) /* TODO: free this memory */, *$4);
		} |
		type CURVED_OPEN STAR ident CURVED_CLOSE CURVED_OPEN func_decl_args CURVED_CLOSE ASSIGN_EQUAL expr
		{
			$$ = new NVariableDeclaration(*(new NFunctionPointerType(*$1, *$7)) /* TODO: free this memory */, *$4, $10);
		} ;
		
ident:
		IDENTIFIER
		{
			$$ = new NIdentifier(*$1);
			delete $1;
		} ;

type:
		type_base
		{
			$$ = new NType($<type>1->name, 0, false);
			delete $1;
		} |
		STRUCT ident
		{
			$$ = new NType($<type>2->name, 0, true);
			delete $2;
		} |
		type_base STAR %prec IREF
		{
			$$ = new NType($<type>1->name, 1, false);
			delete $1;
		} |
		STRUCT ident STAR %prec IREF
		{
			$$ = new NType($<type>2->name, 1, true);
			delete $2;
		} |
		type STAR %prec IREF
		{
			$$ = new NType($<type>1->name, $<type>1->pointerCount + 1, $<type>1->isStruct);
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
		stmt_while |
		stmt_for |
		stmt_return |
		stmt_debug |
		stmt_asm |
		expr SEMICOLON
		{
			$$ = new NExpressionStatement(*$1);
		} ;

block_or_stmt:
		stmt
		{
			$$ = new NBlock();
			$$->statements.push_back($<stmt>1);
		} |
		block
		{
			$$ = $1;
		} ;

stmt_if:
		IF CURVED_OPEN expr CURVED_CLOSE block_or_stmt ELSE block_or_stmt
		{
			$$ = new NIfStatement(*$3, *$5, $7);
		} |
		IF CURVED_OPEN expr CURVED_CLOSE block_or_stmt
		{
			$$ = new NIfStatement(*$3, *$5);
		} ;

stmt_return:
		RETURN expr SEMICOLON
		{
			$$ = new NReturnStatement(*$2);
		} ;

stmt_debug:
		DEBUG expr SEMICOLON
		{
			$$ = new NDebugStatement(*$2);
		} ;

stmt_while:
		WHILE CURVED_OPEN expr CURVED_CLOSE block_or_stmt
		{
			$$ = new NWhileStatement(*$3, *$5);
		} ;

stmt_for:
		FOR CURVED_OPEN expr SEMICOLON expr SEMICOLON expr CURVED_CLOSE block_or_stmt
		{
			$$ = new NForStatement(*$3, *$5, *$7, *$9);
		} ;

stmt_asm:
		ASSEMBLY
		{
			$$ = new NAssemblyStatement(*$1);
			delete $1;
		} ;

fldref:
		expr DOT ident
		{
			$$ = new NStructureResolutionOperator(*$1, *$3, false);
		} |
		fldref assignop expr
		{
			if ($1->cType == "expression-field") // We can't accept NAssignments as the fldref in this case.
				$$ = new NAssignment(*$1, $2, *$3);
			else
				throw new CompilerException("Unable to apply field referencing assignment operation to non-field operator based LHS.");
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
				$$ = new NAssignment(*$1, $2, *$3);
			else
				throw new CompilerException("Unable to apply dereferencing assignment operation to non-dereference operator based LHS.");
		} ;

expr:
		ident assignop expr
		{
			$$ = new NAssignment(*$1, $2, *$3);
		} |
		deref
		{
			$$ = $1;
		} |
		fldref
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
		/*expr DEREFDOT ident
		{
			$$ = new NStructureResolutionOperator(*$1, *$3, true);
		} |*/
		expr binaryop expr
		{
			$$ = new NBinaryOperator(*$1, $2, *$3);
		} |
		BINARY_AND addressable %prec IADDROF
		{
			$$ = new NAddressOfOperator(*$2);
		} |
		CURVED_OPEN expr CURVED_CLOSE
		{
			$$ = $2;
		} ;

addressable:
		ident
		{
			$$ = $1;
		} |
		expr DOT ident
		{
			$$ = new NStructureResolutionOperator(*$1, *$3, false);
		} |
		STAR expr %prec IDEREF
		{
			$$ = new NDereferenceOperator(*$2);
		} ;

numeric:
		NUMBER
		{
			$$ = new NInteger($1);
		} |
		TRUE
		{
			$$ = new NInteger(1);
		} |
		FALSE
		{
			$$ = new NInteger(0);
		} |
		SIZEOF CURVED_OPEN type CURVED_CLOSE
		{
			$$ = new NSizeOfOperator(*$3);
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
		SLASH |
		PERCENT ;

type_base:
		TYPE_VOID
		{
			$$ = new NIdentifier("void");
		} |
		TYPE_CHAR
		{
			$$ = new NIdentifier("char");
		} |
		TYPE_BYTE
		{
			$$ = new NIdentifier("byte");
		} |
		TYPE_INT
		{
			$$ = new NIdentifier("int");
		} |
		TYPE_LONG
		{
			$$ = new NIdentifier("long");
		} |
		TYPE_INT8_T
		{
			$$ = new NIdentifier("int8_t");
		} |
		TYPE_INT16_T
		{
			$$ = new NIdentifier("int16_t");
		} |
		TYPE_INT32_T
		{
			$$ = new NIdentifier("int32_t");
		} |
		TYPE_INT64_T
		{
			$$ = new NIdentifier("int64_t");
		} |
		TYPE_UINT8_T
		{
			$$ = new NIdentifier("uint8_t");
		} |
		TYPE_UINT16_T
		{
			$$ = new NIdentifier("uint16_t");
		} |
		TYPE_UINT32_T
		{
			$$ = new NIdentifier("uint32_t");
		} |
		TYPE_UINT64_T
		{
			$$ = new NIdentifier("uint64_t");
		} ;
