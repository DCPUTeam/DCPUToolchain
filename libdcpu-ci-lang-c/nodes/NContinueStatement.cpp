/**

	File:		NContinueStatement.cpp

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	Patrick Flick

	Description:	Defines the NContinueStatement AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NContinueStatement.h"

AsmBlock* NContinueStatement::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();

	// Add file and line information.
	*block << this->getFileAndLineState();

	std::string contlabel = context.getContinueLabel();
	if (contlabel == std::string(""))
	{
		throw new CompilerException(this->line, this->file, "Invalid continue statement outside of loop.");
	}

	*block <<  "	SET PC, " << contlabel << std::endl;

	return block;
}

AsmBlock* NContinueStatement::reference(AsmGenerator& context)
{
	throw new CompilerException(this->line, this->file, "Unable to get reference to the result of an return statement.");
}
