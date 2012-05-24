/**

	File:		NString.cpp

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	James Rhodes

	Description:	Defines the NString AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NString.h"
#include "TPointer16.h"
#include "TInt16.h"

AsmBlock* NString::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();

	// Add file and line information.
	*block << this->getFileAndLineState();

	// Stop if the assembler doesn't support DAT.
	if (!context.getAssembler().supportsDataInstruction)
		throw new CompilerException(this->line, this->file, "Unable to compile strings without DAT support in assembler.");
	
	// 
	std::string s = this->value;
	std::stringstream outputstr;
	outputstr << "\"";
	for (std::string::const_iterator i = s.begin(), end = s.end(); i != end; ++i)
	{
		unsigned char c = *i;
		switch (c)
		{
		case '"':  outputstr << "\\\"";  break;
		case '\'': outputstr << "\\\'";  break;
		case '\\': outputstr << "\\\\"; break;
		case '\t': outputstr << "\\t";  break;
		case '\r': outputstr << "\\r";  break;
		case '\n': outputstr << "\\n";  break;
		case '\v': outputstr << "\\v";  break;
		case '\f': outputstr << "\\f";  break;
		case '\b': outputstr << "\\b";  break;
		case '\a': outputstr << "\\a";  break;
		case '\0': outputstr << "\\0";  break;
		case '\?': outputstr << "\\\?";  break;
		default : outputstr << c;
		}
	}
	outputstr << "\"";
	
	// Generate a label for the DAT and then output the DAT.
	std::string strlabel = context.getRandomLabel("cstr");
	*block <<	"	SET PC, " << strlabel << "_jmpover" << std::endl;
	*block <<	"	:" << strlabel << " DAT " << outputstr.str() << ", 0" << std::endl;

	// Load the address of the string in register A.
	*block <<	"	:" << strlabel << "_jmpover SET A, " << strlabel << "" << std::endl;

	return block;
}

AsmBlock* NString::reference(AsmGenerator& context)
{
	throw new CompilerException(this->line, this->file, "Unable to get reference to the result of a string literal.");
}

IType* NString::getExpressionType(AsmGenerator& context)
{
	// A string has the type char*.
	// currently char is mapped to int16_t
	IType* t = new TPointer16(new TInt16("char"));
	return t;
}
