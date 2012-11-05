/**

    File:           NEmptyStatement.cpp

    Project:        DCPU-16 Tools
    Component:      LibDCPU-ci-lang-c

    Authors:        James Rhodes, Patrick Flick

    Description:    Defines the NEmptyStatement AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NEmptyStatement.h"
#include <derr.defs.h>

AsmBlock* NEmptyStatement::compile(AsmGenerator& context)
{
    // Generate an empty block.
    AsmBlock* block = new AsmBlock();
    return block;
}

AsmBlock* NEmptyStatement::reference(AsmGenerator& context)
{
    throw new CompilerException(this->line, this->file, "Unable to get reference to the result of an empty statement.");
}

void NEmptyStatement::analyse(AsmGenerator& context, bool reference)
{
    if (reference)
    {
        context.errorList.addError(this->line, this->file, ERR_CC_CANNOT_REFERENCE, " an empty statement");
        return;
    }
}
