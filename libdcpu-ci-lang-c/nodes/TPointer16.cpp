/**

	File:		TPointer16.cpp

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	Patrick Flick

	Description:	Defines the TPointer Type class.

**/

#define YYSTYPE int

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "TPointer16.h"
#include "../parser.hpp"

bool TPointer16::isBasicType() const
{
	return false;
}

bool TPointer16::isPointer() const
{
	return true;
}

bool TPointer16::isConst() const
{
	return this->m_isConst;
}

void TPointer16::setConst()
{
	this->m_isConst = true;
}

std::string TPointer16::getName() const
{
	return "(" + m_pointingTo->getName() + ")*";
}

std::string TPointer16::getInternalName() const
{
	return "ptr16_t";
}


IType* TPointer16::getPointerBaseType()
{
	return this->m_pointingTo;
}


bool TPointer16::implicitCastable(AsmGenerator& context, const IType* toType)
{
	std::string to = toType->getInternalName();
	if (to != "ptr16_t")
	{
		return false;
	}
	else
	{
		TPointer16* otherType = (TPointer16*) toType;
		IType* otherBaseType = otherType->getPointerBaseType();
		IType* myBaseType = this->m_pointingTo;

		// recursively check pointed to types
		return myBaseType->implicitCastable(context, otherBaseType);
	}
}

bool TPointer16::explicitCastable(AsmGenerator& context, const IType* toType)
{
	std::string to = toType->getInternalName();
	if (to == "uint16_t")
	{
		return true;
	}
	else if (to == "int16_t")
	{
		return true;
	}
	else if (to == "ptr16_t")
	{
		return true;
	}
	else
	{
		return false;
	}
}

AsmBlock* TPointer16::implicitCast(AsmGenerator& context, const IType* toType, char a)
{
	if (!this->implicitCastable(context, toType))
	{
		throw new CompilerException(0, "<internal>",
					    "Unable to implicitly cast integer (internal error).");
	}
	// dont do anything (value stays the same)
	AsmBlock* block = new AsmBlock();
	return block;
}

AsmBlock* TPointer16::explicitCast(AsmGenerator& context, const IType* toType, char a)
{
	if (!this->explicitCastable(context, toType))
	{
		throw new CompilerException(0, "<internal>",
					    "Unable to implicitly cast integer (internal error).");
	}
	// TODO if different sizes integers are introduced they have
	//	 to be cast here
	AsmBlock* block = new AsmBlock();
	return block;
}

// these depend on the context
AsmBlock* TPointer16::add(AsmGenerator& context, char pointer, char integer)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	MUL " << integer << ", " << this->m_pointingTo->getWordSize(context) << std::endl;
	*block <<	"	ADD " << pointer << ", " << integer << std::endl;
	return block;
}
AsmBlock* TPointer16::sub(AsmGenerator& context, char pointer, char integer)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	MUL " << integer << ", " << this->m_pointingTo->getWordSize(context) << std::endl;
	*block <<	"	SUB " << pointer << ", " << integer << std::endl;
	return block;
}

AsmBlock* TPointer16::inc(AsmGenerator& context, char ref)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	ADD [" << ref << "], " << this->m_pointingTo->getWordSize(context) << std::endl;
	return block;
}
AsmBlock* TPointer16::dec(AsmGenerator& context, char ref)
{
	AsmBlock* block = new AsmBlock();
	*block <<	"	SUB [" << ref << "], " << this->m_pointingTo->getWordSize(context) << std::endl;
	return block;
}

AsmBlock* TPointer16::compileBinaryOperator(NBinaryOperator* binopNode, AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();

	// get types
	IType* lhsType = binopNode->lhs.getExpressionType(context);
	IType* rhsType = binopNode->rhs.getExpressionType(context);

	// get promoted type and cast values to result type
	IType* pointerType;
	bool bothPointers = false;
	if (lhsType->isPointer())
	{
		pointerType = lhsType;
		if (rhsType->isPointer()) bothPointers = true;

		// If debugging, clear the values as we POP them.
		if (context.isAssemblerDebug())
		{
			// Put the values into A and B and clear the
			// stack positions as we do so.
			*block <<   *(rhsType->popStackCleanReturn(context, 'B'));
			*block <<   *(lhsType->popStackCleanReturn(context, 'A'));
		}
		else
		{
			// Not debugging, put the values into A and B.
			*block <<   *(rhsType->popStackReturn(context, 'B'));
			*block <<   *(lhsType->popStackReturn(context, 'A'));
		}
	}
	else if (rhsType->isPointer() && binopNode->op == ADD)
	{
		// reverse pop

		pointerType = rhsType;
		// If debugging, clear the values as we POP them.
		if (context.isAssemblerDebug())
		{
			// Put the values into A and B and clear the
			// stack positions as we do so.
			*block <<   *(lhsType->popStackCleanReturn(context, 'B'));
			*block <<   *(rhsType->popStackCleanReturn(context, 'A'));
		}
		else
		{
			// Not debugging, put the values into A and B.
			*block <<   *(lhsType->popStackReturn(context, 'B'));
			*block <<   *(rhsType->popStackReturn(context, 'A'));
		}
	}
	else
	{
		throw new CompilerException(binopNode->line, binopNode->file,
					    "Invalid operands to binary operation. (have '"
					    + lhsType->getName() + "' and '" + rhsType->getName() + "')");
	}

	// for comparison use Uint16
	IType* compareType = new TUnsignedInt16();

	// Now do the appropriate operation.
	if (bothPointers)
	{
		switch (binopNode->op)
		{
			case COMPARE_EQUAL:
				*block << *(compareType->eq(context, 'A', 'B'));
				break;
			case COMPARE_NOT_EQUAL:
				*block << *(compareType->neq(context, 'A', 'B'));
				break;
			case COMPARE_LESS_THAN:
				*block << *(compareType->lt(context, 'A', 'B'));
				break;
			case COMPARE_LESS_THAN_EQUAL:
				*block << *(compareType->le(context, 'A', 'B'));
				break;
			case COMPARE_GREATER_THAN:
				*block << *(compareType->gt(context, 'A', 'B'));
				break;
			case COMPARE_GREATER_THAN_EQUAL:
				*block << *(compareType->ge(context, 'A', 'B'));
				break;
			default:
				throw new CompilerException(binopNode->line, binopNode->file,
							    "Invalid operands to binary operation. (have '"
							    + lhsType->getName() + "' and '" + rhsType->getName() + "')");
		}
	}
	else
	{
		switch (binopNode->op)
		{
			case ADD:
				*block << *(pointerType->add(context, 'A', 'B'));
				break;
			case SUBTRACT:
				*block << *(pointerType->sub(context, 'A', 'B'));
				break;
			default:
				throw new CompilerException(binopNode->line, binopNode->file, "Invalid operands to binary operation. (have '" + lhsType->getName() + "' and '" + rhsType->getName() + "')");
		}
	}

	throw new CompilerException(binopNode->line, binopNode->file, "Binary operator not handled by pointer (internal error).");
}

