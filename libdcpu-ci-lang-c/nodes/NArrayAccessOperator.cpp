/**

    File:       NArrayAccessOperator.cpp

    Project:    DCPU-16 Tools
    Component:  LibDCPU-ci-lang-c

    Authors:    James Rhodes

    Description:    Defines the NArrayAccessOperator AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NType.h"
#include "NArrayAccessOperator.h"
#include "TPointer16.h"
#include <derr.defs.h>

AsmBlock* NArrayAccessOperator::compile(AsmGenerator& context)
{
    // We just use our own reference method and then
    // implicitly dereference here.

    // Get our type.
    IType* baseType = this->getExpressionType(context);

    AsmBlock* block = this->reference(context);
    *block << *(baseType->loadFromRef(context, 'A', 'A'));
    return block;
}

AsmBlock* NArrayAccessOperator::reference(AsmGenerator& context)
{
    AsmBlock* block = new AsmBlock();

    // Add file and line information.
    *block << this->getFileAndLineState();

    // Get a reference to the first component of the array access (result
    // will end up in A).
    AsmBlock* exprA = this->exprA.compile(context);
    *block << *exprA;
    *block << *(this->m_pointerType->pushStack(context, 'A'));
    delete exprA;

    // Evaluate the second component of the array access (result
    // will end up in A).
    AsmBlock* exprB = this->exprB.compile(context);
    *block << *exprB;
    delete exprB;

    // get result from exprA from stack
    *block << " SET B, A" << std::endl;
    *block << *(this->m_pointerType->popStackReturn(context, 'A'));

    // Add the two together.
    *block << *(this->m_pointerType->add(context, 'A', 'B'));

    // The memory address is now in A, so we can simply return.
    return block;
}

void NArrayAccessOperator::analyse(AsmGenerator& context, bool reference)
{
    // Get array types
    this->m_baseType = this->getExpressionType(context);
    this->m_pointerType = this->exprA.getExpressionType(context);
    
    this->exprA.analyse(context, false);
    this->exprB.analyse(context, false);
    
    // type checking exprA
    if (!this->m_pointerType->isPointer())
    {
        context.errorList.addError(this->line, this->file, ERR_CC_INVALID_ARRAY_ACCESS_TYPE, this->m_pointerType->getName());
        return;
    }
    
    // get type
    IType* integralType = this->exprB.getExpressionType(context);
    if (!integralType->isBasicType())
    {
        context.errorList.addError(this->line, this->file, ERR_CC_INVALID_ARRAY_ACCESS_OFFSET, integralType->getName());
        return;
    }
}

IType* NArrayAccessOperator::getExpressionType(AsmGenerator& context)
{
    // An dereference operator has the "unpointered" type of it's expression.
    IType* i = this->exprA.getExpressionType(context);

    if (i->getInternalName() == "ptr16_t")
    {
        TPointer16* ptr = (TPointer16*) i;
        IType* baseType = ptr->getPointerBaseType();
        return baseType;
    }
    // FIXME: create (better: not create, get static void* type) pointer to void here:
    else if (i->implicitCastable(context, new TPointer16(new TUnsignedInt16())))
    {
        // FIXME return void?
        return new TUnsignedInt16();
    }
    else
    {
        throw new CompilerException(this->line, this->file, "Attempting to dereference non-pointer type during array access.");
    }
}
