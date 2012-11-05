/**

    File:       NBreakStatement.cpp

    Project:    DCPU-16 Tools
    Component:  LibDCPU-ci-lang-c

    Authors:    Patrick Flick

    Description:    Defines the NBreakStatement AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NBreakStatement.h"
#include <derr.defs.h>

AsmBlock* NBreakStatement::compile(AsmGenerator& context)
{
    AsmBlock* block = new AsmBlock();

    // Add file and line information.
    *block << this->getFileAndLineState();

    *block <<  "    SET PC, " << this->m_breakLabel << std::endl;

    return block;
}

AsmBlock* NBreakStatement::reference(AsmGenerator& context)
{
    throw new CompilerException(this->line, this->file, "Unable to get reference to the result of an break statement.");
}

void NBreakStatement::analyse(AsmGenerator& context, bool reference)
{
    if (reference)
    {
        context.errorList.addError(this->line, this->file, ERR_CC_CANNOT_REFERENCE, " a return statement");
        return;
    }
    
    this->m_breakLabel = context.getBreakLabel();
    if (this->m_breakLabel == std::string(""))
    {
        context.errorList.addError(this->line, this->file, ERR_CC_BREAK_OUTSIDE_OF_LOOP);
        return;
    }
}
