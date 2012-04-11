/**

	File:			NReturnStatement.cpp

	Project:		DCPU-16 Tools
	Component:		Compiler

	Authors:		James Rhodes

	Description:	Defines the NReturnStatement AST class.

**/

#include "../asmgen.h"
#include "NReturnStatement.h"

AsmBlock* NReturnStatement::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();
	
	// Evaluate the expression (the expression will always put
	// it's output in register A).
	AsmBlock* eval = ((NExpression&)this->result).compile(context);
	*block << *eval;
	delete eval;

	// Push the size of this function into X so that
	// _stack_return works.
	*block <<  "	SET X, " << context.m_CurrentFrame->getSize() << std::endl;

	// Return from this function.
	*block <<  "	SET PC, _stack_return" << std::endl;

	return block;
}

AsmBlock* NReturnStatement::reference(AsmGenerator& context)
{
	throw new CompilerException("Unable to get reference to the result of an return statement.");
}