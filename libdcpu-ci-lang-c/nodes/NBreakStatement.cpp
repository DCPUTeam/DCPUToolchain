/**

	File:		NBreakStatement.cpp

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	Patrick Flick

	Description:	Defines the NBreakStatement AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NBreakStatement.h"

AsmBlock* NBreakStatement::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();

	// Add file and line information.
	*block << this->getFileAndLineState();

	std::string brklabel = context.getBreakLabel();
	if (brklabel == std::string(""))
	{
		throw new CompilerException(this->line, this->file, "Invalid break statement outside of loop.");
	}

	*block <<  "	SET PC, " << brklabel << std::endl;

	return block;
}

AsmBlock* NBreakStatement::reference(AsmGenerator& context)
{
	throw new CompilerException(this->line, this->file, "Unable to get reference to the result of an return statement.");
}
