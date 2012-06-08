/**

	File:		NTypedefDeclaration.cpp

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	Patrick Flick

	Description:	Defines the NTypedefDeclaration AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NTypedefDeclaration.h"
#include "TStruct.h"

AsmBlock* NTypedefDeclaration::compile(AsmGenerator& context)
{
	// Create our new block.
	AsmBlock* block = new AsmBlock();
	
	// Get the position of the variable.
	TypePosition result = context.m_CurrentFrame->getPositionOfVariable(this->id.name);

	if (!result.isFound())
		throw new CompilerException(this->line, this->file, "The variable '" + this->id.name + "' was not found in the scope.");
	
	// get variable type
	IType* varType = context.m_CurrentFrame->getTypeOfVariable(this->id.name);
	
	// if this is a struct, it may has to be initialized (arrays within structs)
	if (varType->isStruct())
	{
		// Set the value of the variable directly.
		*block << result.pushAddress('I');
		TStruct* structType = (TStruct*) varType;
		*block << *(structType->initStruct(context, 'I'));
	}
	
	// If we have no initialization expression, we don't need to do anything.
	if (this->initExpr == NULL)
		return block;

	// Add file and line information.
	*block << this->getFileAndLineState();

	// When an expression is evaluated, the result goes into the A register.
	AsmBlock* expr = this->initExpr->compile(context);
	*block << *expr;
	delete expr;

	// get type, it may has to be cast
	IType* exprType = this->initExpr->getExpressionType(context);

	// cast to rhs to lhs type
	if (exprType->implicitCastable(context, varType))
	{
		*block << *(exprType->implicitCast(context, varType, 'A'));
	}
	else
	{
		throw new CompilerException(this->line, this->file,
					    "Unable to implicitly cast '" + exprType->getName()
					    + "' to '" + varType->getName() + "'");
	}

	// Set the value of the variable directly.
	*block << result.pushAddress('I');

	// save the value A to [I]
	*block << *(varType->saveToRef(context, 'A', 'I'));

	return block;
}

AsmBlock* NTypedefDeclaration::reference(AsmGenerator& context)
{
	throw new CompilerException(this->line, this->file, "Unable to get reference to the result of a variable.");
}
