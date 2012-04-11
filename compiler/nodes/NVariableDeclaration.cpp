/**

	File:			NVariableDeclaration.cpp

	Project:		DCPU-16 Tools
	Component:		Compiler

	Authors:		James Rhodes

	Description:	Defines the NVariableDeclaration AST class.

**/

#include "../asmgen.h"
#include "NVariableDeclaration.h"

AsmBlock* NVariableDeclaration::compile(AsmGenerator& context)
{
	// If we have no initialization expression, we don't need to do anything.
	if (this->initExpr == NULL)
		return NULL;

	// Create our new block.
	AsmBlock* block = new AsmBlock();
	
	// When an expression is evaluated, the result goes into the A register.
	AsmBlock* expr = this->initExpr->compile(context);
	*block << *expr;
	delete expr;

	// Get the position of the variable.
	int32_t pos = context.m_CurrentFrame->getPositionOfVariable(this->id.name);
	if (pos == -1)
		throw new CompilerException("The variable '" + this->id.name + "' was not found in the scope.");

	// Set the value of the variable directly.
	std::stringstream sstr;
	if (pos == 0)
		sstr << "[Y]";
	else
		sstr << "[0x" << std::hex << pos << "+Y]";
	*block <<	"	SET " << sstr.str() << ", A" << std::endl;

	return block;
}

AsmBlock* NVariableDeclaration::reference(AsmGenerator& context)
{
	throw new CompilerException("Unable to get reference to the result of a variable.");
}