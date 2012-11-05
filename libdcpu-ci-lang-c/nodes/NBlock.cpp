/**

    File:       NBlock.cpp

    Project:    DCPU-16 Tools
    Component:  LibDCPU-ci-lang-c

    Authors:    James Rhodes

    Description:    Defines the NBlock AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NBlock.h"
#include <derr.defs.h>

AsmBlock* NBlock::compile(AsmGenerator& context)
{
    AsmBlock* block = new AsmBlock();

    // Add file and line information.
    *block << this->getFileAndLineState();

    // Now run through each instruction and generate code for it.
    for (StatementList::iterator i = this->statements.begin(); i != this->statements.end(); i++)
    {
        AsmBlock* inst = (*i)->compile(context);
        *block << *inst;
        delete inst;
    }

    return block;
}

AsmBlock* NBlock::reference(AsmGenerator& context)
{
    throw new CompilerException(this->line, this->file, "Unable to get reference to the result of a block node.");
}

void NBlock::analyse(AsmGenerator& context, bool reference)
{
    if (reference)
    {
        context.errorList.addError(this->line, this->file, ERR_CC_CANNOT_REFERENCE, " the result of a binary operation");
        return;
    }
    
    for (StatementList::iterator i = this->statements.begin(); i != this->statements.end(); i++)
    {
        (*i)->analyse(context, false);
    }
}

IType* NBlock::getExpressionType(AsmGenerator& context)
{
    // A block of statements has no type.
    return new NType(NType::VoidType);
}
