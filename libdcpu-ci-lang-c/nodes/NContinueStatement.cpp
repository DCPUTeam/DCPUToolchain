/**

    File:       NContinueStatement.cpp

    Project:    DCPU-16 Tools
    Component:  LibDCPU-ci-lang-c

    Authors:    Patrick Flick

    Description:    Defines the NContinueStatement AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NContinueStatement.h"
#include <derr.defs.h>

AsmBlock* NContinueStatement::compile(AsmGenerator& context)
{
    AsmBlock* block = new AsmBlock();

    // Add file and line information.
    *block << this->getFileAndLineState();

    *block <<  "    SET PC, " << this->m_continueLabel << std::endl;

    return block;
}

AsmBlock* NContinueStatement::reference(AsmGenerator& context)
{
    throw new CompilerException(this->line, this->file, "Unable to get reference to the result of an return statement.");
}

void NContinueStatement::analyse(AsmGenerator& context, bool reference)
{
    if (reference)
    {
        context.errorList.addError(this->line, this->file, ERR_CC_CANNOT_REFERENCE, " a continue statement");
        return;
    }
    
    this->m_continueLabel = context.getContinueLabel();
    if (this->m_continueLabel == std::string(""))
    {
        context.errorList.addError(this->line, this->file, ERR_CC_CONTINUE_OUTSIDE_OF_LOOP);
        return;
    }
}
