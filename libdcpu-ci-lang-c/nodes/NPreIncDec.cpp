/**

	File:		NPreIncDec.cpp

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	Patrick Flick

	Description:	Defines the NPreIncDec AST class.

**/

// Prevents importing all of the class dependencies in
// the parser header so that we only get the token values.
#define YYSTYPE int

#include "NInteger.h"
#include <AsmGenerator.h>
#include <CompilerException.h>
#include "../parser.hpp"
#include "NPreIncDec.h"

AsmBlock* NPreIncDec::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();

	// When an expression is evaluated, the result goes into the A register.
	AsmBlock* expr = this->expr.compile(context);
	*block <<   *expr;
	delete expr;



	// Now do the appropriate operation.
	switch (this->op)
	{
		case INCREMENT:
			*block <<	"	ADD A, 1" << std::endl;
			break;

		case DECREMENT:
			*block <<	"	SUB A, 1" << std::endl;
			break;

		default:
			throw new CompilerException("Unknown Pre-Increase-Decrease operation requested.");
	}

	// Move the value into register B.
	*block <<	"	SET B, A" << std::endl;

	// When an assignment expression is referenced, the memory
	// address of the target goes into A.
	AsmBlock* las = this->expr.reference(context);
	*block << *las;
	delete las;

	// Move the value into the target address.
	*block <<	"	SET [A], B" << std::endl;
	
	// The operation returns the value itself in A
	*block <<	"	SET A, B" << std::endl;
	
	return block;
}

AsmBlock* NPreIncDec::compilePostOperators(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();
	return block;
}


AsmBlock* NPreIncDec::reference(AsmGenerator& context)
{
	throw new CompilerException("Unable to get reference to the result of an Pre-Increment.");
}

IType& NPreIncDec::getExpressionType(AsmGenerator& context)
{
	// An assignment has the type of it's LHS.
	return this->expr.getExpressionType(context);
}
