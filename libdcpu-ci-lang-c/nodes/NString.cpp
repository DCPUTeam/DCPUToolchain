/**

	File:           NString.cpp

	Project:        DCPU-16 Tools
	Component:      LibDCPU-ci-lang-c

	Authors:        James Rhodes

	Description:    Defines the NString AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NString.h"
#include "NInteger.h"

AsmBlock* NString::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();

	// Stop if the assembler doesn't support DAT.
	if (!context.getAssembler().supportsDataInstruction)
		throw new CompilerException("Unable to compile strings without DAT support in assembler.");

	// Generate a label for the DAT and then output the DAT.
	std::string strlabel = context.getRandomLabel("cstr");
	*block <<	"	SET PC, " << strlabel << "_jmpover" << std::endl;
	*block <<	"	:" << strlabel << " DAT \"" << this->value << "\", 0" << std::endl;

	// Load the address of the string in register A.
	*block <<	"	:" << strlabel << "_jmpover SET A, " << strlabel << "" << std::endl;

	return block;
}

AsmBlock* NString::reference(AsmGenerator& context)
{
	throw new CompilerException("Unable to get reference to the result of a string literal.");
}

IType& NString::getExpressionType(AsmGenerator& context)
{
	// A string has the type char*.
	NType t = NType(NInteger::CharType);
	t.pointerCount += 1;
	return t;
}
