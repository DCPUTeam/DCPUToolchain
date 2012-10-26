/**

    File:       NIntegerLiteral.cpp

    Project:    DCPU-16 Tools
    Component:  LibDCPU-ci-lang-c

    Authors:    James Rhodes

    Description:    Defines the NIntegerLiteral AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NIntegerLiteral.h"
#include "TInt16.h"

AsmBlock* NIntegerLiteral::compile(AsmGenerator& context)
{
    AsmBlock* block = new AsmBlock();

    // Add file and line information.
    *block << this->getFileAndLineState();

    // Load the numeric value in register A.
    *block <<   "   SET A, " << (const size_t)this->value << std::endl;

    return block;
}

AsmBlock* NIntegerLiteral::reference(AsmGenerator& context)
{
    throw new CompilerException(this->line, this->file, "Unable to get reference to the result of a numeric literal.");
}

IType* NIntegerLiteral::getExpressionType(AsmGenerator& context)
{
    // All literals are currently 16-bit integers.
    return new TInt16("int");
}
