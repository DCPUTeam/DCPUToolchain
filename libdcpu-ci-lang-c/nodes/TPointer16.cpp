/**

	File:		TPointer16.cpp

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	Patrick Flick

	Description:	Defines the TPointer Type class.

**/


#include "TPointer16.h"

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


bool TPointer16::implicitCastable(const IType* toType)
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
		return myBaseType->implicitCastable(otherBaseType);
	}
}

bool TPointer16::explicitCastable(const IType* toType)
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

AsmBlock* TPointer16::implicitCast(const IType* toType, char a)
{
	if (!this->implicitCastable(toType))
	{
		throw new CompilerException(0, "<internal>", 
		"Unable to implicitly cast integer (internal error).");
	}
	// dont do anything (value stays the same)
	AsmBlock* block = new AsmBlock();
	return block;
}

AsmBlock* TPointer16::explicitCast(const IType* toType, char a)
{
	if (!this->explicitCastable(toType))
	{
		throw new CompilerException(0, "<internal>", 
		"Unable to implicitly cast integer (internal error).");
	}
	// TODO if different sizes integers are introduced they have
	//       to be cast here
	AsmBlock* block = new AsmBlock();
	return block;
}


