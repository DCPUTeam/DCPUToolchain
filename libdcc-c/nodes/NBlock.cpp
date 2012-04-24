/**

	File:			NBlock.cpp

	Project:		DCPU-16 Tools
	Component:		Compiler

	Authors:		James Rhodes

	Description:	Defines the NBlock AST class.

**/

#include <asmgen.h>
#include "NBlock.h"

AsmBlock* NBlock::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();
	
	// Now run through each instruction and generate code for it.
	for (StatementList::iterator i = this->statements.begin(); i != this->statements.end(); i++)
	{
		AsmBlock* inst = (*i)->compile(context);
		*block << *inst;
		delete inst;
	}

	return block;
}

AsmBlock* NBlock::reference(AsmGenerator& context)
{
	throw new CompilerException("Unable to get reference to the result of a block node.");
}

IType& NBlock::getExpressionType(AsmGenerator& context)
{
	// A block of statements has no type.
	return NType::VoidType;
}