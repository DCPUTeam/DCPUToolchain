/**

	File:			NExpressionStatement.cpp

	Project:		DCPU-16 Tools
	Component:		Compiler

	Authors:		James Rhodes

	Description:	Defines the NExpressionStatement AST class.

**/

#include "../asmgen.h"
#include "NExpressionStatement.h"

AsmBlock* NExpressionStatement::compile(AsmGenerator& context)
{
	return this->expression.compile(context);
}

AsmBlock* NExpressionStatement::reference(AsmGenerator& context)
{
	throw new CompilerException("Unable to get reference to the result of an expression statement.");
}