/**

	File:		NDereferenceOperator.cpp

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	James Rhodes

	Description:	Defines the NDereferenceOperator AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NType.h"
#include "NDereferenceOperator.h"
#include "TPointer16.h"
#include "TUint16.h"

AsmBlock* NDereferenceOperator::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();

	// Add file and line information.
	*block << this->getFileAndLineState();

	// When an expression is evaluated, the result goes into the A register.
	AsmBlock* expr = this->expr.compile(context);

	// An dereference operator has the "unpointered" type of it's expression.
	IType* i = this->expr.getExpressionType(context);
	
	IType* baseType;
	if (i->getInternalName() == "ptr16_t")
	{
		TPointer16* ptr = (TPointer16*) i;
		IType* baseType = ptr->getPointerBaseType();	
	}
	// FIXME: create (better: not create, get static void* type) pointer to void here:
	else if (i->implicitCastable(new TPointer16(new TUint16())))
	{
		// FIXME return void?
		baseType= new TUint16();
	}
	else
	{
		throw new CompilerException(this->line, this->file, "Attempting to dereference non-pointer type during type resolution.");
	}

	// Dereference the value.
	*block <<   *expr;
	*block <<   *(baseType->loadFromRef('A', 'A')) << std::endl;
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

IType* NDereferenceOperator::getExpressionType(AsmGenerator& context)
{
	// An dereference operator has the "unpointered" type of it's expression.
	IType* i = this->expr.getExpressionType(context);
	
	if (i->getInternalName() == "ptr16_t")
	{
		TPointer16* ptr = (TPointer16*) i;
		IType* baseType = ptr->getPointerBaseType();
		return baseType;	
	}
	// FIXME: create (better: not create, get static void* type) pointer to void here:
	else if (i->implicitCastable(new TPointer16(new TUint16())))
	{
		// FIXME return void?
		return new TUint16();
	}
	else
	{
		throw new CompilerException(this->line, this->file, "Attempting to dereference non-pointer type during type resolution.");
	}
}
