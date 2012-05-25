%{

/**

	File:           parser.y

	Project:        DCPU-16 Tools
	Component:      LibDCPU-ci-lang-c

	Authors:        James Rhodes
	                Michael Gerhaeuser

	Description:    Defines parser for the compiler.

**/

class NAssignmentExpression;
class NInteger;

#include <lexfix.h>
#include <bstring.h>
#include <cstdio>
#include <vector>
#include <AsmGenerator.h>
#include <CompilerException.h>
#include "nodes/NArrayAccessOperator.h"
#include "nodes/NBlock.h"
#include "nodes/NDeclarations.h"
#include "nodes/NArrayDeclaration.h"
#include "nodes/NFunctionDeclaration.h"
#include "nodes/NStructureDeclaration.h"
#include "nodes/NEmptyStatement.h"
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
#include "nodes/NUnaryOperator.h"
#include "nodes/NPreIncDec.h"
#include "nodes/NPostIncDec.h"
#include "nodes/NAddressOfOperator.h"
#include "nodes/NIntegerLiteral.h"
#include "nodes/NSizeOfOperator.h"
#include "nodes/NCharacter.h"
#include "nodes/NString.h"
#include "nodes/TInt16.h"
#include "nodes/TPointer16.h"
#include "nodes/TStruct.h"


// Turn on verbose error messages.
#define YYERROR_VERBOSE

// Defines the root node.
NDeclarations* program;

// YY-stuff.
extern int yylex();
extern int yycolumn;
extern bstring yyfilename;
void yyerror(const char *str);

%}

%union {
	NBlock* block;
	NExpression* expr;
	NStatement* stmt;
	NIdentifier* ident;
	NIntegerLiteral* numeric;
	IType* type;
	NDeclarations* decls;
	NFunctionDeclaration* function;
	NStructureDeclaration* structure;
	NVariableDeclaration *variable;
	NArrayDeclaration *array;
	std::vector<NExpression*> *exprvec;
	std::vector<NDeclaration*> *declvec;
	std::vector<NVariableDeclaration*> *varvec;
	std::vector<uint16_t> *dimvec;
	std::string* string;
	const char* data;
	long number;
	int token;
}

/* TOKENS: File and line information */
%token <token> LINE_FILE

/* TOKENS: Error token for Flex to throw an error */
%token <token> ERROR

/* TOKENS: Identifiers, numbers and basic lexical components */
%token <token> CURVED_OPEN CURVED_CLOSE BRACE_OPEN BRACE_CLOSE SQUARE_OPEN SQUARE_CLOSE COMMA
%token <token> STAR SEMICOLON DOT STRUCT
%token <number> NUMBER
%token <string> IDENTIFIER CHARACTER STRING ASSEMBLY

/* TOKENS: Assignment, equivilance and mathematical operators */
%token <token> ASSIGN_EQUAL ASSIGN_ADD ASSIGN_SUBTRACT ASSIGN_MULTIPLY ASSIGN_DIVIDE
%token <token> ASSIGN_MOD ASSIGN_BAND ASSIGN_BOR ASSIGN_BXOR ASSIGN_SHL ASSIGN_SHR
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
%token <token> TYPE_VOID TYPE_CHAR TYPE_SHORT TYPE_INT TYPE_LONG TYPE_FLOAT TYPE_DOUBLE CONST UNSIGNED SIGNED

/* TYPES */
%type <type> type type_base
%type <ident> ident
%type <expr> expr numeric string character deref fldref addressable arrayref
%type <varvec> func_decl_args struct_decl_args
%type <exprvec> call_args array_init_list
%type <dimvec> array_dims
%type <decls> program prog_decl
%type <function> func_decl
%type <structure> struct_decl
%type <variable> var_decl
%type <array> array_decl
%type <block> block stmts block_or_stmt
%type <stmt> stmt stmt_if stmt_return stmt_while stmt_for stmt_debug stmt_asm
%type <token> assignop

/* OPERATOR PRECEDENCE (LOWEST -> HIGHEST) */
%left COMMA
%right ASSIGN_EQUAL ASSIGN_ADD ASSIGN_SUBTRACT ASSIGN_MULTIPLY ASSIGN_DIVIDE ASSIGN_MOD ASSIGN_BAND ASSIGN_BOR ASSIGN_BXOR ASSIGN_SHL ASSIGN_SHR
%left BOOLEAN_OR
%left BOOLEAN_AND
%left BINARY_OR
%left BINARY_XOR
%left BINARY_AND
%left COMPARE_EQUAL COMPARE_NOT_EQUAL
%left COMPARE_LESS_THAN COMPARE_LESS_THAN_EQUAL COMPARE_GREATER_THAN COMPARE_GREATER_THAN_EQUAL
%left BINARY_LEFT_SHIFT BINARY_RIGHT_SHIFT
%left ADD SUBTRACT
%left STAR SLASH PERCENT
%right IPREINC IPREDEC IUNARYPLUS IUNARYMINUS NEGATE BITWISE_NEGATE IADDROF
%left IPOSTINC IPOSTDEC DOT DEREFDOT

/* START POINT */
%start program

%%

program:
		/* No code */
		{
			$$ = new NDeclarations();
			program = $$;
		} |
		prog_decl
		{
			$$ = $1;
			program = $1;
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
		array_decl SEMICOLON
		{
			$$ = new NDeclarations();
			$$->definitions.push_back($<array>1);
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
		} |
		prog_decl array_decl SEMICOLON
		{
			$1->definitions.push_back($<array>2);
		};

func_decl:
		type ident CURVED_OPEN func_decl_args CURVED_CLOSE block
		{
			$$ = new NFunctionDeclaration($1, *$2, *$4, $6);
			//delete $4;
		} |
		type ident CURVED_OPEN func_decl_args CURVED_CLOSE SEMICOLON
		{
			$$ = new NFunctionDeclaration($1, *$2, *$4, NULL);
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
		/* TODO arrays (and function pointers) within structs */
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
			$$ = new NVariableDeclaration($1, *$2);
		} |
		type ident ASSIGN_EQUAL expr
		{
			$$ = new NVariableDeclaration($1, *$2, $4);
		} |
		type CURVED_OPEN STAR ident CURVED_CLOSE CURVED_OPEN func_decl_args CURVED_CLOSE
		{
			$$ = new NVariableDeclaration((new NFunctionPointerType($1, *$7)) /* TODO: free this memory */, *$4);
		} |
		type CURVED_OPEN STAR ident CURVED_CLOSE CURVED_OPEN func_decl_args CURVED_CLOSE ASSIGN_EQUAL expr
		{
			$$ = new NVariableDeclaration((new NFunctionPointerType($1, *$7)) /* TODO: free this memory */, *$4, $10);
		} ;

array_decl:
		/* array declarations */
		type ident array_dims
		{
			$$ = new NArrayDeclaration($1, *$2, $3, NULL);
		} |
		type ident array_dims ASSIGN_EQUAL BRACE_OPEN array_init_list BRACE_CLOSE
		{
			$$ = new NArrayDeclaration($1, *$2, $3, $6);
		} ;

array_dims:
		SQUARE_OPEN NUMBER SQUARE_CLOSE
		{
			$$ = new DimensionsList();
			$$->push_back((uint16_t)$2);
		} |
		array_dims SQUARE_OPEN NUMBER SQUARE_CLOSE
		{
			$$ = $1;
			$$->push_back((uint16_t)$3);
		} ;

array_init_list:
			/* empty */
		{
			$$ = new ExpressionList();
		} |
		expr
		{
			$$ = new ExpressionList();
			$$->push_back($1);
		} |
		/* ignore further Braces */
		BRACE_OPEN array_init_list BRACE_CLOSE
		{
			$$ = $2;
		} |
		array_init_list COMMA array_init_list
		{
			$1->insert( $1->end(), $3->begin(), $3->end() );
			$$ = $1;
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
			$$ = $<type>1;
		} |
		CONST type
		{
			$$ = $2;
			$$->setConst();
		} |
		type CONST
		{
			$$ = $1;
			$$->setConst();
		} |
		SIGNED type
		{
			$$ = $2;
			$$->setSigned(true);
		} |
		type SIGNED
		{
			$$ = $1;
			$$->setSigned(true);
		} |
		UNSIGNED type
		{
			$$ = $2;
			$$->setSigned(false);
		} |
		type UNSIGNED
		{
			$$ = $1;
			$$->setSigned(false);
		} |
		STRUCT ident
		{
			$$ = new TStruct($<ident>2->name);
			delete $2;
		} |
		/*
		type_base STAR %prec IREF
		{
			$$ = new TPointer16($<type>1);
		} |
		STRUCT ident STAR %prec IREF
		{
			$$ = new TPointer16(new TStruct($<ident>2->name));
			delete $2;
		} |
		*/
		// pointer to type
		type STAR %prec IREF
		{
			$$ = new TPointer16($<type>1);
		} |
		// const pointer to type
		type STAR CONST %prec IREF
		{
			$$ = new TPointer16($<type>1);
			$$->setConst();
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
		SEMICOLON
		{
			$$ = new NEmptyStatement();
		} |
		var_decl SEMICOLON
		{
			$$ = $1;
		} |
		array_decl SEMICOLON
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
				throw new CompilerException(yylineno, (const char*)yyfilename->data, "Unable to apply field referencing assignment operation to non-field operator based LHS.");
		} ;

arrayref:
		expr SQUARE_OPEN expr SQUARE_CLOSE
		{
			$$ = new NArrayAccessOperator(*$1, *$3);
		} |
		arrayref assignop expr
		{
			if ($1->cType == "expression-arrayaccess") // We can't accept NAssignments as the arrayref in this case.
				$$ = new NAssignment(*$1, $2, *$3);
			else
				throw new CompilerException(yylineno, (const char*)yyfilename->data, "Unable to apply array access assignment operation to non-array access operator based LHS.");
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
				throw new CompilerException(yylineno, (const char*)yyfilename->data, "Unable to apply dereferencing assignment operation to non-dereference operator based LHS.");
		} ;

expr:
		ident assignop expr
		{
			$$ = new NAssignment(*$1, $2, *$3);
		} |
		arrayref
		{
			$$ = $1;
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
		
		/* Increment and Decrement */
		INCREMENT expr %prec IPREINC
		{
			$$ = new NPreIncDec($1, *$2);
		} |
		DECREMENT expr %prec IPREDEC
		{
			$$ = new NPreIncDec($1, *$2);
		} |
		
		expr INCREMENT %prec IPOSTINC
		{
			$$ = new NPostIncDec(*$1, $2);
		} |
		expr DECREMENT %prec IPOSTDEC
		{
			$$ = new NPostIncDec(*$1, $2);
		} |
		
				
		/* Boolean Binary Operators */	
		
		expr BOOLEAN_AND expr
		{
			$$ = new NBinaryOperator(*$1, $2, *$3);
		} |
		expr BOOLEAN_OR expr
		{
			$$ = new NBinaryOperator(*$1, $2, *$3);
		} |
				
		/* Binary Binary Operators */	
		
		expr BINARY_AND expr
		{
			$$ = new NBinaryOperator(*$1, $2, *$3);
		} |
		expr BINARY_OR expr
		{
			$$ = new NBinaryOperator(*$1, $2, *$3);
		} |
		expr BINARY_XOR expr
		{
			$$ = new NBinaryOperator(*$1, $2, *$3);
		} |
		expr BINARY_LEFT_SHIFT expr
		{
			$$ = new NBinaryOperator(*$1, $2, *$3);
		} |
		expr BINARY_RIGHT_SHIFT expr
		{
			$$ = new NBinaryOperator(*$1, $2, *$3);
		} |
		
		/* Arithmetic Binary Operators */		
		
		expr SUBTRACT expr
		{
			$$ = new NBinaryOperator(*$1, $2, *$3);
		} |
		expr ADD expr
		{
			$$ = new NBinaryOperator(*$1, $2, *$3);
		} |
		expr STAR expr
		{
			$$ = new NBinaryOperator(*$1, $2, *$3);
		} |
		expr SLASH expr
		{
			$$ = new NBinaryOperator(*$1, $2, *$3);
		} |
		expr PERCENT expr
		{
			$$ = new NBinaryOperator(*$1, $2, *$3);
		} |
		
		/* Comparing Binary Operators */		
		
		expr COMPARE_EQUAL expr
		{
			$$ = new NBinaryOperator(*$1, $2, *$3);
		} |
		expr COMPARE_NOT_EQUAL expr
		{
			$$ = new NBinaryOperator(*$1, $2, *$3);
		} |
		expr COMPARE_LESS_THAN expr
		{
			$$ = new NBinaryOperator(*$1, $2, *$3);
		} |
		expr COMPARE_LESS_THAN_EQUAL expr
		{
			$$ = new NBinaryOperator(*$1, $2, *$3);
		} |
		expr COMPARE_GREATER_THAN expr
		{
			$$ = new NBinaryOperator(*$1, $2, *$3);
		} |
		expr COMPARE_GREATER_THAN_EQUAL expr
		{
			$$ = new NBinaryOperator(*$1, $2, *$3);
		} |
		
		BINARY_AND addressable %prec IADDROF
		{
			$$ = new NAddressOfOperator(*$2);
		} |
		
		
		/* unary operators TODO IPOSTINC IPOSTDEC IPREINC IPREDEC  */
		SUBTRACT expr %prec IUNARYMINUS
		{
			$$ = new NUnaryOperator($1,*$2);
		} |
		ADD expr %prec IUNARYPLUS
		{
			$$ = new NUnaryOperator($1,*$2);
		} |
		NEGATE expr
		{
			$$ = new NUnaryOperator($1,*$2);
		} |
		BITWISE_NEGATE expr
		{
			$$ = new NUnaryOperator($1,*$2);
		} |

		/* ( expr ) */
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
			/* TODO implement other unsigned, long,     */
			/*      and unsigned long (u,l,ul) literals */
			$$ = new NIntegerLiteral($1);
		} |
		TRUE
		{
			$$ = new NIntegerLiteral(1);
		} |
		FALSE
		{
			$$ = new NIntegerLiteral(0);
		} |
		SIZEOF CURVED_OPEN type CURVED_CLOSE
		{
			$$ = new NSizeOfOperator($3);
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
		ASSIGN_DIVIDE |
		ASSIGN_MOD |
		ASSIGN_BAND |
		ASSIGN_BOR |
		ASSIGN_BXOR |
		ASSIGN_SHL |
		ASSIGN_SHR;

type_base:
		TYPE_VOID
		{
			//$$ = new NType("void", 0, false);
			// TODO Void Type
			$$ = new TInt16("void");
		} |
		TYPE_CHAR
		{
			//$$ = new NType("char", 0, false);
			$$ = new TInt16("char");
		} |
		TYPE_SHORT
		{
			//$$ = new NType("byte", 0, false);
			$$ = new TInt16("byte");
		} |
		TYPE_INT
		{
			//$$ = new NType("int", 0, false);
			$$ = new TInt16("int");
		} |
		TYPE_LONG
		{
			//$$ = new NType("long", 0, false);
			$$ = new TInt16("long");
		}  |
		TYPE_FLOAT
		{
			//$$ = new NIdentifier("float");
			throw new CompilerException(yylineno, (const char*)yyfilename->data, "Data type 'float' not yet supported.");
		}  |
		TYPE_DOUBLE
		{
			//$$ = new NIdentifier("double");
			throw new CompilerException(yylineno, (const char*)yyfilename->data, "Data type 'double' not yet supported.");
		}  |
		TYPE_LONG TYPE_DOUBLE
		{
			// $$ = new NIdentifier("long double");
			throw new CompilerException(yylineno, (const char*)yyfilename->data, "Data type 'long double' not yet supported.");
		};

%%

#include "lexer.hpp"
#include <cassert>

void yyerror(const char *str)
{
	assert(yyfilename != NULL);
	fprintf(stderr,"error at line %i of '%s': %s\n", yylineno, yyfilename->data, str);
}
