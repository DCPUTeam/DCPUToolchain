/**

	File:		NAssemblyStatement.cpp

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	James Rhodes

	Description:	Defines the NAssemblyStatement AST class.

**/

#include <iostream>
#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NAssemblyStatement.h"

AsmBlock* NAssemblyStatement::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();

	// Directly insert the assembly code.
	*block << this->asmcode << std::endl;

	return block;
}

AsmBlock* NAssemblyStatement::reference(AsmGenerator& context)
{
	throw new CompilerException("Unable to get reference to the result of inline assembly.");
}
