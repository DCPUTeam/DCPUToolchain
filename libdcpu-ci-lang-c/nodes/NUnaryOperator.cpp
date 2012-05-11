/**

	File:		NUnaryOperator.cpp

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	Patrick Flick

	Description:	Defines the NUnaryOperator AST class.

**/

// Prevents importing all of the class dependencies in
// the parser header so that we only get the token values.
#define YYSTYPE int

#include "NInteger.h"
#include <AsmGenerator.h>
#include <CompilerException.h>
#include "../parser.hpp"
#include "NUnaryOperator.h"

AsmBlock* NUnaryOperator::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();
	
	// Add file and line information.
	*block << this->getFileAndLineState();

	// When an expression is evaluated, the result goes into the A register.
	AsmBlock* rhs = this->rhs.compile(context);

	// Move the value into B
	*block <<   *rhs;
	*block <<	"	SET B, A" << std::endl;
	delete rhs;

	// Now do the appropriate operation.
	switch (this->op)
	{
		case ADD:
			/* TODO not sure why we even have this */
			*block <<	"	SET A, B" << std::endl;
			break;

		/* unary negative:  "A = -B" */
		case SUBTRACT:	
			// A = 0 - B
			*block <<	"	SET A, 0x0" << std::endl;
			*block <<	"	SUB A, B" << std::endl;
			break;

		/* unary bitwise negate:  "A = ~B" */
		case BITWISE_NEGATE:
			*block <<	"	SET A, 0xffff" << std::endl;
			*block <<	"	XOR A, B" << std::endl;
			break;
		/* boolean negate: A = !B  */
		case NEGATE:
			*block <<	"	IFN B, 0x0" << std::endl;
			*block <<	"		SET B, 0x1" << std::endl;
			*block <<	"	SET A, 0x1" << std::endl;
			*block <<	"	XOR A, B" << std::endl;
			break;
			
		default:
			throw new CompilerException("Unknown unary operations requested.");
	}

	return block;
}

AsmBlock* NUnaryOperator::reference(AsmGenerator& context)
{
	throw new CompilerException("Unable to get reference to the result of an unary operator.");
}

IType& NUnaryOperator::getExpressionType(AsmGenerator& context)
{
	// A binary operator has the type of each of it's expressions.
	NType rType = (NType&)this->rhs.getExpressionType(context);

	return rType;
}
