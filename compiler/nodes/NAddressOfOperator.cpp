/**

	File:			NAddressOfOperator.cpp

	Project:		DCPU-16 Tools
	Component:		Compiler

	Authors:		James Rhodes

	Description:	Defines the NAddressOfOperator AST class.

**/

#include "../asmgen.h"
#include "NType.h"
#include "NAddressOfOperator.h"

AsmBlock* NAddressOfOperator::compile(AsmGenerator& context)
{
	// Just return the address of the value.
	return this->expr.reference(context);
}

AsmBlock* NAddressOfOperator::reference(AsmGenerator& context)
{
	throw new CompilerException("Unable to get reference to the result of an address-of operator.");
}

NType& NAddressOfOperator::getExpressionType(AsmGenerator& context)
{
	// The address of operator has the type of it's expression as a pointer.
	NType t = NType(this->expr.getExpressionType(context));
	t.pointerCount += 1;
	return t;
}