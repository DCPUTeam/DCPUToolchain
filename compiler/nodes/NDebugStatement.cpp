/**

	File:			NDebugStatement.cpp

	Project:		DCPU-16 Tools
	Component:		Compiler

	Authors:		James Rhodes

	Description:	Defines the NDebugStatement AST class.

**/

#include "../asmgen.h"
#include "NDebugStatement.h"

AsmBlock* NDebugStatement::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();
	
	// Evaluate the expression (the expression will always put
	// it's output in register A).
	AsmBlock* eval = ((NExpression&)this->result).compile(context);
	*block << *eval;
	delete eval;

	// Perform a debug halt so the value of the expression
	// can be inspected in an emulator.
	*block <<  "	SET PC, _halt_debug" << std::endl;

	return block;
}

AsmBlock* NDebugStatement::reference(AsmGenerator& context)
{
	throw new CompilerException("Unable to get reference to the result of a debug statement.");
}