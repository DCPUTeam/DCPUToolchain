/**

	File:		NVariableDeclaration.cpp

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	James Rhodes

	Description:	Defines the NVariableDeclaration AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NVariableDeclaration.h"

AsmBlock* NVariableDeclaration::compile(AsmGenerator& context)
{
	// If we have no initialization expression, we don't need to do anything.
	if (this->initExpr == NULL)
		return NULL;

	// Create our new block.
	AsmBlock* block = new AsmBlock();

	// Add file and line information.
	*block << this->getFileAndLineState();

	// When an expression is evaluated, the result goes into the A register.
	AsmBlock* expr = this->initExpr->compile(context);
	*block << *expr;
	delete expr;

	// Get the position of the variable.
	TypePosition result = context.m_CurrentFrame->getPositionOfVariable(this->id.name);

	if (!result.isFound())
		throw new CompilerException(this->line, this->file, "The variable '" + this->id.name + "' was not found in the scope.");

	// Set the value of the variable directly.
	*block << result.pushAddress('I');
	*block <<	"	SET [I], A" << std::endl;

	return block;
}

AsmBlock* NVariableDeclaration::reference(AsmGenerator& context)
{
	throw new CompilerException(this->line, this->file, "Unable to get reference to the result of a variable.");
}
