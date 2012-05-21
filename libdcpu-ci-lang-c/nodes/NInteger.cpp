/**

	File:		NInteger.cpp

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	James Rhodes

	Description:	Defines the NInteger AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NInteger.h"

NType internal_CharType("char", 0, false, false, false);
NType internal_ShortType("short", 0, false, false, false);
NType internal_IntegerType("int", 0, false, false, false);
NType internal_LongType("long", 0, false, false, false);
NType& NInteger::CharType(internal_CharType);
NType& NInteger::ShortType(internal_ShortType);
NType& NInteger::IntegerType(internal_IntegerType);
NType& NInteger::LongType(internal_LongType);

AsmBlock* NInteger::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();

	// Add file and line information.
	*block << this->getFileAndLineState();

	// Load the numeric value in register A.
	*block <<	"	SET A, " << (const size_t)this->value << std::endl;

	return block;
}

AsmBlock* NInteger::reference(AsmGenerator& context)
{
	throw new CompilerException(this->line, this->file, "Unable to get reference to the result of a numeric literal.");
}

IType* NInteger::getExpressionType(AsmGenerator& context)
{
	// All literals are currently signed 16-bit integers.
	return new NType(NInteger::IntegerType);
}
