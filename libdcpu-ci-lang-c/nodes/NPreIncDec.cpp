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

	// We have to compile and reference, but the reference function allready compiles
	// and evaluates, thus we use the reference not the value.
	AsmBlock* reference = this->expr.reference(context);
	*block <<   *reference;
	delete reference;

	*block <<	"	SET B, A" << std::endl;
	*block <<	"	SET A, [B]" << std::endl;

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


	// Move the value into the target address.
	*block <<	"	SET [B], A" << std::endl;

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
