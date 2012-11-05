/**

    File:           NSizeOfOperator.cpp

    Project:        DCPU-16 Tools
    Component:      LibDCPU-ci-lang-c

    Authors:        James Rhodes, Patrick Flick

    Description:    Defines the NSizeOfOperator AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NSizeOfOperator.h"
#include <derr.defs.h>

AsmBlock* NSizeOfOperator::compile(AsmGenerator& context)
{
    AsmBlock* block = new AsmBlock();

    // Add file and line information.
    *block << this->getFileAndLineState();

    // Load the size of the type into register A.
    *block <<   "   SET A, " << (uint16_t)(this->value->getWordSize(context)) << std::endl;

    return block;
}

AsmBlock* NSizeOfOperator::reference(AsmGenerator& context)
{
    throw new CompilerException(this->line, this->file, "Unable to get reference to the result of the sizeof() operator.");
}

void NSizeOfOperator::analyse(AsmGenerator& context, bool reference)
{
    if (reference)
    {
        context.errorList.addError(this->line, this->file, ERR_CC_CANNOT_REFERENCE, " a size of operator");
        return;
    }
}
