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

AsmBlock* NAddressOfOperator::compile(AsmGenerator& context)
{
    // Just return the address of the value.
    return this->expr.reference(context);
}

AsmBlock* NAddressOfOperator::reference(AsmGenerator& context)
{
    throw new CompilerException(this->line, this->file, "Unable to get reference to the result of an address-of operator.");
}

IType* NAddressOfOperator::getExpressionType(AsmGenerator& context)
{
    // The address of operator has the type of it's expression as a pointer.
    IType* i = this->expr.getExpressionType(context);
    IType* t = new TPointer16(i);
    return t;
}
