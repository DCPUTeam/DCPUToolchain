/**

    File:       NCharacter.cpp

    Project:    DCPU-16 Tools
    Component:  LibDCPU-ci-lang-c

    Authors:    James Rhodes

    Description:    Defines the NCharacter AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NCharacter.h"
#include "TInt16.h"
#include <derr.defs.h>

AsmBlock* NCharacter::compile(AsmGenerator& context)
{
    AsmBlock* block = new AsmBlock();

    // Add file and line information.
    *block << this->getFileAndLineState();

    // Load the numeric value in register A.
    *block << " SET A, " << (uint8_t)((char)this->value[0]) << "" << std::endl;

    return block;
}

AsmBlock* NCharacter::reference(AsmGenerator& context)
{
    throw new CompilerException(this->line, this->file, "Unable to get reference to a character node.");
}

void NCharacter::analyse(AsmGenerator& context, bool reference)
{
    if (reference)
    {
        context.errorList.addError(this->line, this->file, ERR_CC_CANNOT_REFERENCE, " a char literal");
        return;
    }
}

IType* NCharacter::getExpressionType(AsmGenerator& context)
{
    // A character has the type int16.
    return new TInt16("char");
}
