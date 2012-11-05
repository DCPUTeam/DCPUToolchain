/**

    File:       NExpressionStatement.cpp

    Project:    DCPU-16 Tools
    Component:  LibDCPU-ci-lang-c

    Authors:    James Rhodes

    Description:    Defines the NExpressionStatement AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NExpressionStatement.h"
#include <derr.defs.h>

AsmBlock* NExpressionStatement::compile(AsmGenerator& context)
{
    return this->expression.compile(context);
}

AsmBlock* NExpressionStatement::reference(AsmGenerator& context)
{
    throw new CompilerException(this->line, this->file, "Unable to get reference to the result of an expression statement.");
}

void NExpressionStatement::analyse(AsmGenerator& context, bool reference)
{
    if (reference)
    {
        context.errorList.addError(this->line, this->file, ERR_CC_CANNOT_REFERENCE, " an expression statement");
        return;
    }
    
    this->expression.analyse(context, false);
}
