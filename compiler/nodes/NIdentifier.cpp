/**

	File:			NIdentifier.cpp

	Project:		DCPU-16 Tools
	Component:		Compiler

	Authors:		James Rhodes

	Description:	Defines the NIdentifier AST class.

**/

#include "../asmgen.h"
#include "NIdentifier.h"

AsmBlock* NIdentifier::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();

	// Get the position of the variable.
	int32_t pos = context.m_CurrentFrame->getPositionOfVariable(this->name);
	if (pos == -1)
		throw new CompilerException("The variable '" + this->name + "' was not found in the scope.");

	// Load the value of the variable into register A.
	std::stringstream sstr;
	if (pos == 0)
		sstr << "[Y]";
	else
		sstr << "[0x" << std::hex << pos << "+Y]";
	*block <<	"	SET A, " << sstr.str() << std::endl;

	return block;
}

AsmBlock* NIdentifier::reference(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();

	// Get the position of the variable.
	int32_t pos = context.m_CurrentFrame->getPositionOfVariable(this->name);
	if (pos == -1)
		throw new CompilerException("The variable '" + this->name + "' was not found in the scope.");

	// Load the position of the variable into register A.
	*block <<	"	SET A, Y" << std::endl;
	if (pos != 0)
		*block <<	"	ADD A, " << pos << std::endl;

	return block;
}

NType& NIdentifier::getExpressionType(AsmGenerator& context)
{
	// Search the current context for the variable with
	// this name and return it's type.
	NType* type = context.m_CurrentFrame->getTypeOfVariable(this->name);
	if (type == NULL)
		throw new CompilerException("Unable to resolve variable " + this->name + " when determining type information.");
	else
		return *type;
}