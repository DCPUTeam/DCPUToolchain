/**

	File:			NAssignment.cpp

	Project:		DCPU-16 Tools
	Component:		Compiler

	Authors:		James Rhodes

	Description:	Defines the NAssignment AST class.

**/

// Prevents importing all of the class dependencies in
// the parser header so that we only get the token values.
#define YYSTYPE int

#include "NInteger.h"
#include <AsmGenerator.h>
#include <CompilerException.h>
#include "../parser.hpp"
#include "NAssignment.h"

AsmBlock* NAssignment::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();
	
	// We only need to both to push the value of the LHS if we're
	// doing a relative adjustment.
	if (this->op != ASSIGN_EQUAL)
	{
		// When an expression is evaluated, the result goes into the A register.
		AsmBlock* lhs = this->lhs.compile(context);

		// Move the value onto the stack.
		*block <<   *lhs;
		*block <<	"	SET PUSH, A" << std::endl;
		delete lhs;
	}
	else
		// We still need to push a value so our POPs work in order.
		*block <<	"	SET PUSH, 0" << std::endl;

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
	case ASSIGN_EQUAL:
		*block <<	"	SET A, B" << std::endl;
		break;
	case ASSIGN_ADD:
		*block <<	"	ADD A, B" << std::endl;
		break;
	case ASSIGN_SUBTRACT:
		*block <<	"	SUB A, B" << std::endl;
		break;
	case ASSIGN_MULTIPLY:
		*block <<	"	MUL A, B" << std::endl;
		break;
	case ASSIGN_DIVIDE:
		*block <<	"	DIV A, B" << std::endl;
		break;
	default:
		throw new CompilerException("Unknown assignment operation requested.");
	}

	// Move the value into register B.
	*block <<	"	SET B, A" << std::endl;

	// When an assignment expression is referenced, the memory
	// address of the target goes into A.
	AsmBlock* las = this->lhs.reference(context);
	*block << *las;
	delete las;

	// Move the value into the target address.
	*block <<	"	SET [A], B" << std::endl;

	return block;
}

AsmBlock* NAssignment::reference(AsmGenerator& context)
{
	throw new CompilerException("Unable to get reference to the result of an assignment.");
}

IType& NAssignment::getExpressionType(AsmGenerator& context)
{
	// An assignment has the type of it's LHS.
	return this->lhs.getExpressionType(context);
}
