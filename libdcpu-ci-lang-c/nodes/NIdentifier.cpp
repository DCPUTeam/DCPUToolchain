/**

    File:       NIdentifier.cpp

    Project:    DCPU-16 Tools
    Component:  LibDCPU-ci-lang-c

    Authors:    James Rhodes

    Description:    Defines the NIdentifier AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NIdentifier.h"
#include "NType.h"
#include <derr.defs.h>
#include <iostream>
#include "TInt16.h"

AsmBlock* NIdentifier::compile(AsmGenerator& context)
{
    AsmBlock* block = new AsmBlock();

    // Add file and line information.
    *block << this->getFileAndLineState();

    // Load the value of the variable into register A.
    *block << this->m_varPos.pushAddress('I');
    *block << *(this->m_varType->loadFromRef(context, 'I', 'A'));

    return block;
}

AsmBlock* NIdentifier::reference(AsmGenerator& context)
{
    AsmBlock* block = new AsmBlock();

    // Add file and line information.
    *block << this->getFileAndLineState();

    // Load the position of the variable into register A.
    *block << this->m_varPos.pushAddress('A');

    return block;
}

void NIdentifier::analyse(AsmGenerator& context, bool reference)
{
    // Get the position and type of the variable.
    this->m_varPos = context.symbolTable->getPositionOfVariable(this->name);
    
    if (!this->m_varPos.isFound())
    {
        context.errorList.addError(this->line, this->file, ERR_CC_VARIABLE_NOT_IN_SCOPE, this->name);
        return;
    }
    
    this->m_varType = context.symbolTable->getTypeOfVariable(this->name);

    if ((!reference) && this->m_varPos.isFunction())
    {
        context.errorList.addError(this->line, this->file, ERR_CC_VALUE_OF_FUNCTION, this->name);
        return;
    }
}

IType* NIdentifier::getExpressionType(AsmGenerator& context)
{
    // Search the current context for the variable with
    // this name and return it's type.
    IType* type = context.symbolTable->getTypeOfVariable(this->name);

    if (type == NULL)
    {
        context.errorList.addError(this->line, this->file, ERR_CC_VARIABLE_NOT_IN_SCOPE, this->name);
        // return some void type (we need a class called "unknown type")
        return new TInt16("void");
    }
    else
    {
        // Return type
        return type;
    }
}
