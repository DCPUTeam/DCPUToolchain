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
	AsmBlock* block = new AsmBlock();
	AsmBlock* expr = this->expression.compile(context);
	AsmBlock* exprPost = this->expression.compilePostOperators(context);
	*block << *expr;
	*block << *exprPost;
	delete expr;
	delete exprPost;
	return block;
	
}

AsmBlock* NExpressionStatement::reference(AsmGenerator& context)
{
	throw new CompilerException("Unable to get reference to the result of an expression statement.");
}
