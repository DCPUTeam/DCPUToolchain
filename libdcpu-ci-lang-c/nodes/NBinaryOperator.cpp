/**

	File:		NBinaryOperator.cpp

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	James Rhodes
			Michael Gerhaeuser

	Description:	Defines the NBinaryOperator AST class.

**/

// Prevents importing all of the class dependencies in
// the parser header so that we only get the token values.
#define YYSTYPE int

#include "NInteger.h"
#include <AsmGenerator.h>
#include <CompilerException.h>
#include "../parser.hpp"
#include "NBinaryOperator.h"

AsmBlock* NBinaryOperator::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();

	// Add file and line information.
	*block << this->getFileAndLineState();

	// When an expression is evaluated, the result goes into the A register.
	AsmBlock* lhs = this->lhs.compile(context);

	// Move the value onto the stack.
	*block <<   *lhs;
	*block <<	"	SET PUSH, A" << std::endl;
	delete lhs;

	// When an expression is evaluated, the result goes into the A register.
	AsmBlock* rhs = this->rhs.compile(context);

	// Move the value onto the stack.
	*block <<   *rhs;
	*block <<	"	SET PUSH, A" << std::endl;
	delete rhs;

	// If debugging, clear the values as we POP them.
	if (context.isAssemblerDebug())
	{
		// Put the values into A and B and clear the
		// stack positions as we do so.
		*block <<	"	SET B, PEEK" << std::endl;
		*block <<	"	SET PEEK, 0" << std::endl;
		*block <<	"	ADD SP, 1" << std::endl;
		*block <<	"	SET A, PEEK" << std::endl;
		*block <<	"	SET PEEK, 0" << std::endl;
		*block <<	"	ADD SP, 1" << std::endl;
	}
	else
	{
		// Not debugging, put the values into A and B.
		*block <<	"	SET B, POP" << std::endl;
		*block <<	"	SET A, POP" << std::endl;
	}

	// Now do the appropriate operation.
	switch (this->op)
	{
		case ADD:
			*block <<	"	ADD A, B" << std::endl;
			break;

		case SUBTRACT:
			*block <<	"	SUB A, B" << std::endl;
			break;

		case STAR:
			*block <<	"	MUL A, B" << std::endl;
			break;

		case SLASH:
			*block <<	"	DIV A, B" << std::endl;
			break;

		case PERCENT:
			*block <<	"	MOD A, B" << std::endl;
			break;

		case BOOLEAN_AND:
			*block <<	"	IFN A, 0x0" << std::endl;
			*block <<	"		SET A, 0x1" << std::endl;
			*block <<	"	IFN B, 0x0" << std::endl;
			*block <<	"		SET B, 0x1" << std::endl;
			*block <<	"	XOR A, B" << std::endl;
			*block <<	"	XOR A, 0x1" << std::endl;
			break;

		case BOOLEAN_OR:
			*block <<	"	IFN A, 0x0" << std::endl;
			*block <<	"		SET A, 0x1" << std::endl;
			*block <<	"	IFN B, 0x0" << std::endl;
			*block <<	"		SET B, 0x1" << std::endl;
			*block <<	"	BOR A, B" << std::endl;
			*block <<	"	XOR A, 0x1" << std::endl;
			break;

		case BINARY_AND:
			*block <<	"	AND A, B" << std::endl;
			break;

		case BINARY_OR:
			*block <<	"	BOR A, B" << std::endl;
			break;

		case BINARY_XOR:
			*block <<	"	XOR A, B" << std::endl;
			break;

		case BINARY_LEFT_SHIFT:
			*block <<	"	SHL A, B" << std::endl;
			break;

		case BINARY_RIGHT_SHIFT:
			*block <<	"	SHR A, B" << std::endl;
			break;

		case COMPARE_EQUAL:
			*block <<	"	SUB A, B" << std::endl;
			*block <<	"	IFN EX, 0x0" << std::endl;
			*block <<	"		SET A, 0x1" << std::endl;
			*block <<	"	IFN A, 0x0" << std::endl;
			*block <<	"		SET A, 0x1" << std::endl;
			*block <<	"	XOR A, 0x1" << std::endl;
			break;

		case COMPARE_NOT_EQUAL:
			*block <<	"	SUB A, B" << std::endl;
			*block <<	"	IFN EX, 0x0" << std::endl;
			*block <<	"		SET A, 0x1" << std::endl;
			*block <<	"	IFN A, 0x0" << std::endl;
			*block <<	"		SET A, 0x1" << std::endl;
			break;

		case COMPARE_LESS_THAN:
			*block <<	"	SUB B, A" << std::endl;
			*block <<	"	IFE EX, 0x1" << std::endl;
			*block <<	"		SET B, 0x0" << std::endl;
			*block <<	"	IFN B, 0x0" << std::endl;
			*block <<	"		SET B, 0x1" << std::endl;
			*block <<	"	SET A, B" << std::endl;
			break;

		case COMPARE_LESS_THAN_EQUAL:
			*block <<	"	SUB B, A" << std::endl;
			*block <<	"	IFE EX, 0x1" << std::endl;
			*block <<	"		SET B, 0x0" << std::endl;
			*block <<	"	IFE EX, 0x0" << std::endl;
			*block <<	"		SET B, 0x1" << std::endl;
			break;
			*block <<	"	SET A, B" << std::endl;

		case COMPARE_GREATER_THAN:
			*block <<	"	SUB A, B" << std::endl;
			*block <<	"	IFE EX, 0x1" << std::endl;
			*block <<	"		SET A, 0x0" << std::endl;
			*block <<	"	IFN A, 0x0" << std::endl;
			*block <<	"		SET A, 0x1" << std::endl;
			break;

		case COMPARE_GREATER_THAN_EQUAL:
			*block <<	"	SUB A, B" << std::endl;
			*block <<	"	IFE EX, 0x1" << std::endl;
			*block <<	"		SET A, 0x0" << std::endl;
			*block <<	"	IFE EX, 0x0" << std::endl;
			*block <<	"		SET A, 0x1" << std::endl;
			break;

		default:
			throw new CompilerException("Unknown binary operations requested.");
	}

	return block;
}

AsmBlock* NBinaryOperator::reference(AsmGenerator& context)
{
	throw new CompilerException("Unable to get reference to the result of an binary operator.");
}

IType* NBinaryOperator::getExpressionType(AsmGenerator& context)
{
	// A binary operator has the type of each of it's expressions.
	IType* lType = this->lhs.getExpressionType(context);
	IType* rType = this->rhs.getExpressionType(context);

	// FIXME: We need a proper type system allowing for implicit
	//        casts between reasonable values (currently this won't
	//        even permit you to cast between int and int16_t).
	//if (lType != rType)
	//	throw new CompilerException("Unable to implicitly cast from " + lType.name + " to " + rType.name + ".");

	return lType;
}
