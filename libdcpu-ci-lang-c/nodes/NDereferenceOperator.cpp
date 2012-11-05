/**

    File:       NDereferenceOperator.cpp

    Project:    DCPU-16 Tools
    Component:  LibDCPU-ci-lang-c

    Authors:    James Rhodes

    Description:    Defines the NDereferenceOperator AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NType.h"
#include "NDereferenceOperator.h"
#include "TPointer16.h"
#include "TInt16.h"
#include <derr.defs.h>

AsmBlock* NDereferenceOperator::compile(AsmGenerator& context)
{
    AsmBlock* block = new AsmBlock();

    // Add file and line information.
    *block << this->getFileAndLineState();

    // When an expression is evaluated, the result goes into the A register.
    AsmBlock* expr = this->expr.compile(context);

    // Dereference the value.
    *block <<   *expr;
    *block <<   *(this->m_baseType->loadFromRef(context, 'A', 'A')) << std::endl;
    delete expr;

    return block;
}

AsmBlock* NDereferenceOperator::reference(AsmGenerator& context)
{
    // We want the value of the expression because we're using it as
    // a memory address in assignment, hence we don't actually dereference
    // the value.
    return this->expr.compile(context);
}

void NDereferenceOperator::analyse(AsmGenerator& context, bool reference)
{
    this->expr.analyse(context, false);
    
    // An dereference operator has the "unpointered" type of it's expression.
    this->m_baseType = this->getExpressionType(context);
}

IType* NDereferenceOperator::getExpressionType(AsmGenerator& context)
{
    // An dereference operator has the "unpointered" type of it's expression.
    IType* i = this->expr.getExpressionType(context);

    if (i->getInternalName() == "ptr16_t")
    {
        TPointer16* ptr = (TPointer16*) i;
        IType* baseType = ptr->getPointerBaseType();
        return baseType;
    }
    else
    {
        context.errorList.addError(this->line, this->file, ERR_CC_DEREF_NON_POINTER);
        return new TInt16("void");
    }
}
