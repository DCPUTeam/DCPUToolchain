/**

	File:           NDereferenceOperator.cpp

	Project:        DCPU-16 Tools
	Component:      LibDCPU-ci-lang-c

	Authors:        James Rhodes

	Description:    Defines the NDereferenceOperator AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NType.h"
#include "NDereferenceOperator.h"

AsmBlock* NDereferenceOperator::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();
	
	// When an expression is evaluated, the result goes into the A register.
	AsmBlock* expr = this->expr.compile(context);

	// Dereference the value.
	*block <<   *expr;
	*block <<	"	SET A, [A]" << std::endl;
	delete expr;

	return block;
}

AsmBlock* NDereferenceOperator::reference(AsmGenerator& context)
{
	// We want the value of the expression because we're using it as
	// a memory address in assignment, hence we don't actually dereference
	// the value.
	return this->expr.compile(context);
}

IType& NDereferenceOperator::getExpressionType(AsmGenerator& context)
{
	// An dereference operator has the "unpointered" type of it's expression.
	NType t = NType((NType&)this->expr.getExpressionType(context));
	if (t.pointerCount > 0)
		t.pointerCount -= 1;
	else
		throw new CompilerException("Attempting to dereference non-pointer type during type resolution.");
	return t;
}
