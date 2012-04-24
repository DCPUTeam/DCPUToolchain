/**

	File:			NSizeOfOperator.cpp

	Project:		DCPU-16 Tools
	Component:		Compiler

	Authors:		James Rhodes

	Description:	Defines the NSizeOfOperator AST class.

**/

#include <asmgen.h>
#include "NSizeOfOperator.h"

AsmBlock* NSizeOfOperator::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();

	// Load the size of the type into register A.
	*block <<	"	SET A, " << (uint16_t)(this->value.getWordSize(context)) << std::endl;

	return block;
}

AsmBlock* NSizeOfOperator::reference(AsmGenerator& context)
{
	throw new CompilerException("Unable to get reference to the result of the sizeof() operator.");
}