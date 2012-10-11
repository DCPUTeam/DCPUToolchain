/**

	File:		NBinaryOperator.cpp

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	James Rhodes
			Michael Gerhaeuser
			Patrick Flick

	Description:	Defines the NBinaryOperator AST class.

**/

// Prevents importing all of the class dependencies in
// the parser header so that we only get the token values.
#define YYSTYPE int

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "parser.hpp"
#include "NBinaryOperator.h"
#include "TGenericBasicType.h"
#include "TPointer16.h"

AsmBlock* NBinaryOperator::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();

	// Add file and line information.
	*block << this->getFileAndLineState();

	// When an expression is evaluated, the result goes into the A register.
	AsmBlock* lhs = this->lhs.compile(context);

	// get lhs type
	IType* lhsType = this->lhs.getExpressionType(context);

	// Move the value onto the stack.
	*block <<   *lhs;
	*block <<   *(lhsType->pushStack(context, 'A'));
	delete lhs;

	// When an expression is evaluated, the result goes into the A register.
	AsmBlock* rhs = this->rhs.compile(context);

	// get rhs type
	IType* rhsType = this->rhs.getExpressionType(context);

	// Move the value onto the stack.
	*block <<   *rhs;
	*block <<   *(rhsType->pushStack(context, 'A'));
	delete rhs;

	// Binary Operations are either possible on two basic types
	// or a pointer and a basic type
	// Check if both types are of a basic type
	bool isPointerOp = false;
	if (rhsType->isPointer() || lhsType->isPointer())
	{
		isPointerOp = true;
	}
	else if ((!rhsType->isBasicType()) || (!lhsType->isBasicType()))
	{
		throw new CompilerException(this->line, this->file,
					    "Invalid operands to binary operation. (have '"
					    + lhsType->getName() + "' and '" + rhsType->getName() + "')");
	}

	if (!isPointerOp)
	{
		*block << *(TGenericBasicType::compileBinaryOperator(this, context));
	}
	else
	{
		*block << *(TPointer16::compileBinaryOperator(this, context));
	}

	return block;
}

AsmBlock* NBinaryOperator::reference(AsmGenerator& context)
{
	throw new CompilerException(this->line, this->file, "Unable to get reference to the result of an binary operator.");
}

IType* NBinaryOperator::getExpressionType(AsmGenerator& context)
{
	// A binary operator has the common (promoted) type of it's expressions.
	IType* lType = this->lhs.getExpressionType(context);
	IType* rType = this->rhs.getExpressionType(context);

	return TGenericBasicType::promoteTypes(lType, rType);
}
