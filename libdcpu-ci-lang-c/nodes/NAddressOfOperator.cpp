/**

    File:       NAddressOfOperator.cpp

    Project:    DCPU-16 Tools
    Component:  LibDCPU-ci-lang-c

    Authors:    James Rhodes

    Description:    Defines the NAddressOfOperator AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NType.h"
#include "NAddressOfOperator.h"
#include "TPointer16.h"
#include <derr.defs.h>

AsmBlock* NAddressOfOperator::compile(AsmGenerator& context)
{
    // Just return the address of the value.
    return this->expr.reference(context);
}

AsmBlock* NAddressOfOperator::reference(AsmGenerator& context)
{
    throw new CompilerException(this->line, this->file, "INTERNAL: Unable to get reference to the result of an address-of operator.");
}

void NAddressOfOperator::analyse(AsmGenerator& context, bool reference)
{
    if (reference)
    {
        context.errorList.addError(this->line, this->file, ERR_CC_CANNOT_REFERENCE_REFOP);
        return;
    }
    else
        expr.analyse(context, true);
}

IType* NAddressOfOperator::getExpressionType(AsmGenerator& context)
{
    // The address of operator has the type of it's expression as a pointer.
    IType* i = this->expr.getExpressionType(context);
    IType* t = new TPointer16(i);
    return t;
}
