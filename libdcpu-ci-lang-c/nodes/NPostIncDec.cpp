/**

	File:		NPostIncDec.cpp

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	Patrick Flick

	Description:	Defines the NPostIncDec AST class.

**/

// Prevents importing all of the class dependencies in
// the parser header so that we only get the token values.
#define YYSTYPE int

#include "NInteger.h"
#include <AsmGenerator.h>
#include <CompilerException.h>
#include "../parser.hpp"
#include "NPostIncDec.h"

AsmBlock* NPostIncDec::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();

	// We have to compile and reference, but the reference function allready compiles
	// and evaluates, thus we use the reference not the value.
	AsmBlock* reference = this->expr.reference(context);
	*block <<   *reference;
	delete reference;
	
	*block <<	"	SET B, A" << std::endl;
	// return old value in A
	*block <<	"	SET A, [B]" << std::endl;
	
	// increment/decrement
	switch (this->op)
	{
		case INCREMENT:
			*block <<	"	ADD [B], 1" << std::endl;
			break;

		case DECREMENT:
			*block <<	"	SUB [B], 1" << std::endl;
			break;

		default:
			throw new CompilerException("Unknown Post-Increase-Decrease operation requested.");
	}
	
	return block;
}

AsmBlock* NPostIncDec::reference(AsmGenerator& context)
{
	throw new CompilerException("Unable to get reference to the result of an Post-Increment.");
}

IType& NPostIncDec::getExpressionType(AsmGenerator& context)
{
	// An assignment has the type of it's LHS.
	return this->expr.getExpressionType(context);
}
