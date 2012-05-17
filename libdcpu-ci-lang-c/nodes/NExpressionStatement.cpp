/**

	File:		NExpressionStatement.cpp

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	James Rhodes

	Description:	Defines the NExpressionStatement AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NExpressionStatement.h"

AsmBlock* NExpressionStatement::compile(AsmGenerator& context)
{
	return this->expression.compile(context);
}

AsmBlock* NExpressionStatement::reference(AsmGenerator& context)
{
	throw new CompilerException(this->line, this->file, "Unable to get reference to the result of an expression statement.");
}
