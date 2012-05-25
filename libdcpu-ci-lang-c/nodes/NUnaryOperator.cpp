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

	// get type
	IType* rhsType = this->rhs.getExpressionType(context);

	if (!rhsType->isBasicType())
	{
		throw new CompilerException(this->line, this->file,
					    "Invalid operand to unary operation. (have '"
					    + rhsType->getName() + "')");
	}

	// Move the value into A
	*block <<   *rhs;
	delete rhs;

	// Now do the appropriate operation.
	AsmBlock* compiledOp;
	switch (this->op)
	{
		case ADD:
			/* TODO integer promotion */
			compiledOp = rhsType->plus(context, 'A');
			break;

			/* unary negative:  "A = -A" */
		case SUBTRACT:
			// A = 0 - A
			compiledOp = rhsType->minus(context, 'A');
			break;

			/* unary bitwise negate:  "A = ~A" */
		case BITWISE_NEGATE:
			compiledOp = rhsType->bnot(context, 'A');
			break;
			/* boolean negate: A = !A  */
		case NEGATE:
			compiledOp = rhsType->lnot(context, 'A');
			break;

		default:
			throw new CompilerException(this->line, this->file, "Unknown unary operations requested.");
	}
	*block << *compiledOp;

	return block;
}

AsmBlock* NUnaryOperator::reference(AsmGenerator& context)
{
	throw new CompilerException(this->line, this->file, "Unable to get reference to the result of an unary operator.");
}

IType* NUnaryOperator::getExpressionType(AsmGenerator& context)
{
	// A unary operator has the type of it's expression.
	return this->rhs.getExpressionType(context);
}
