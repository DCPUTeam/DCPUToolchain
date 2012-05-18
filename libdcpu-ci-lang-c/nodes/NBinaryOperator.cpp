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

#include "NInteger.h"
#include <AsmGenerator.h>
#include <CompilerException.h>
#include "../parser.hpp"
#include "NBinaryOperator.h"
#include "TGenericBasicType.h"

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
	*block <<   *(lhsType->pushStack('A'));
	delete lhs;

	// When an expression is evaluated, the result goes into the A register.
	AsmBlock* rhs = this->rhs.compile(context);

	// get rhs type
	IType* rhsType = this->rhs.getExpressionType(context);
	
	// Move the value onto the stack.
	*block <<   *rhs;
	*block <<   *(rhsType->pushStack('A'));
	delete rhs;

	// If debugging, clear the values as we POP them.
	if (context.isAssemblerDebug())
	{
		// Put the values into A and B and clear the
		// stack positions as we do so.
		*block <<   *(rhsType->popStackCleanReturn('B'));
		*block <<   *(lhsType->popStackCleanReturn('A'));
	}
	else
	{
		// Not debugging, put the values into A and B.
		*block <<   *(rhsType->popStackReturn('B'));
		*block <<   *(lhsType->popStackReturn('A'));
	}
	
	// Check if both types are of a basic type
	if ((!rhsType->isBasicType()) || (!lhsType->isBasicType()))
	{
		throw new CompilerException(this->line, this->file, 
		"Invalid operands to binary operation. (have '"
		+ lhsType->getName() + "' and '" + rhsType->getName() + "')");
	}
	
	// get promoted type and cast values to result type
	IType* commonType = TGenericBasicType::promoteTypes(lhsType, rhsType);
	if (lhsType != commonType)
	{
		// cast lhsType
		if (lhsType->implicitCastable(commonType))
		{
			*block << *(lhsType->implicitCast(commonType, 'A'));
		}
		else
		{
			throw new CompilerException(this->line, this->file, 
			"Unable to implicitly cast '" + lhsType->getName()
			+ "' to '" + commonType->getName() + "'");
		}
	}
	else if (rhsType != commonType)
	{
		// cast rhsType
		if (rhsType->implicitCastable(commonType))
		{
			*block << *(rhsType->implicitCast(commonType, 'B'));
		}
		else
		{
			throw new CompilerException(this->line, this->file, 
			"Unable to implicitly cast '" + rhsType->getName()
			+ "' to '" + commonType->getName() + "'");
		}
	}

	// Now do the appropriate operation.
	switch (this->op)
	{
		case ADD:
			*block << *(commonType->add('A','B'));
			break;

		case SUBTRACT:
			*block << *(commonType->sub('A','B'));
			break;

		case STAR:
			*block << *(commonType->mul('A','B'));
			break;

		case SLASH:
			*block << *(commonType->div('A','B'));
			break;

		case PERCENT:
			*block << *(commonType->mod('A','B'));
			break;

		case BOOLEAN_AND:
			*block << *(commonType->land('A','B'));
			break;

		case BOOLEAN_OR:
			*block << *(commonType->lor('A','B'));
			break;

		case BINARY_AND:
			*block << *(commonType->band('A','B'));
			break;

		case BINARY_OR:
			*block << *(commonType->bor('A','B'));
			break;

		case BINARY_XOR:
			*block << *(commonType->bxor('A','B'));
			break;

		case BINARY_LEFT_SHIFT:
			*block << *(commonType->shl('A','B'));
			break;

		case BINARY_RIGHT_SHIFT:
			*block << *(commonType->shr('A','B'));
			break;

		case COMPARE_EQUAL:
			*block << *(commonType->eq('A','B'));
			break;

		case COMPARE_NOT_EQUAL:
			*block << *(commonType->neq('A','B'));
			break;

		case COMPARE_LESS_THAN:
			*block << *(commonType->lt('A','B'));
			break;

		case COMPARE_LESS_THAN_EQUAL:
			*block << *(commonType->le('A','B'));
			break;

		case COMPARE_GREATER_THAN:
			*block << *(commonType->gt('A','B'));
			break;

		case COMPARE_GREATER_THAN_EQUAL:
			*block << *(commonType->ge('A','B'));
			break;

		default:
			throw new CompilerException(this->line, this->file, "Unknown binary operations requested.");
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
