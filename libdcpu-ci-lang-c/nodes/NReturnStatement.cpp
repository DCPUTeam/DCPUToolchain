/**

	File:		NReturnStatement.cpp

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	James Rhodes

	Description:	Defines the NReturnStatement AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NReturnStatement.h"

AsmBlock* NReturnStatement::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();

	// Add file and line information.
	*block << this->getFileAndLineState();

	// Evaluate the expression (the expression will always put
	// it's output in register A).
	AsmBlock* eval = ((NExpression&)this->result).compile(context);
	*block << *eval;
	delete eval;

	// Free locals.
	*block <<  "	ADD SP, " << context.m_CurrentFrame->getLocalsSize() << std::endl;

	// Return from this function.
	*block <<  "	SET X, " << context.m_CurrentFrame->getParametersSize() << std::endl;
	*block <<  "	SET PC, _stack_callee_return" << std::endl;

	return block;
}

AsmBlock* NReturnStatement::reference(AsmGenerator& context)
{
	throw new CompilerException(this->line, this->file, "Unable to get reference to the result of an return statement.");
}
