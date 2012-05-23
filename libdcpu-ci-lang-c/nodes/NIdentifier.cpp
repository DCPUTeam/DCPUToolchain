/**

	File:		NIdentifier.cpp

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	James Rhodes

	Description:	Defines the NIdentifier AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NIdentifier.h"
#include "NType.h"

AsmBlock* NIdentifier::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();

	// Add file and line information.
	*block << this->getFileAndLineState();

	// Get the position and type of the variable.
	TypePosition result = context.m_CurrentFrame->getPositionOfVariable(this->name);
	IType* type = context.m_CurrentFrame->getTypeOfVariable(this->name);

	if (!result.isFound())
		throw new CompilerException(this->line, this->file, "The variable '" + this->name + "' was not found in the scope.");

	if (result.isFunction())
		throw new CompilerException(this->line, this->file, "Can not get value representation of function '" + this->name + "'; did you want a reference instead?");

	// Load the value of the variable into register A.
	*block << result.pushAddress('I');
	*block << *(type->loadFromRef(context, 'I', 'A'));
	//*block <<	"	SET A, [I]" << std::endl;

	return block;
}

AsmBlock* NIdentifier::reference(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();

	// Add file and line information.
	*block << this->getFileAndLineState();

	// Get the position of the variable.
	TypePosition result = context.m_CurrentFrame->getPositionOfVariable(this->name);

	if (!result.isFound())
		throw new CompilerException(this->line, this->file, "The variable '" + this->name + "' was not found in the scope.");

	// Load the position of the variable into register A.
	*block << result.pushAddress('A');

	return block;
}

IType* NIdentifier::getExpressionType(AsmGenerator& context)
{
	// Search the current context for the variable with
	// this name and return it's type.
	IType* type = context.m_CurrentFrame->getTypeOfVariable(this->name);

	if (type == NULL)
		throw new CompilerException(this->line, this->file, "Unable to resolve variable '" + this->name + "' when determining type information (does the variable exist?).");
	else
	{
		// Return a copy of it since all results from
		// getExpressionType are delete'd by the caller.
		NType* ntype = (NType*)type;
		return new NType(*ntype);
	}
}
