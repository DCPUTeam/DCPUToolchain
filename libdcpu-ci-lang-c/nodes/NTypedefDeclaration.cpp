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

	return block;
}

AsmBlock* NTypedefDeclaration::reference(AsmGenerator& context)
{
	throw new CompilerException(this->line, this->file, "Unable to get reference to the result of a variable.");
}
