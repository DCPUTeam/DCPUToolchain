/**

    File:           NPreIncDec.cpp

    Project:        DCPU-16 Tools
    Component:      LibDCPU-ci-lang-c

    Authors:        Patrick Flick

    Description:    Defines the NPreIncDec AST class.

**/

// Prevents importing all of the class dependencies in
// the parser header so that we only get the token values.
#define YYSTYPE int

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "parser.hpp"
#include "NPreIncDec.h"
#include <derr.defs.h>

AsmBlock* NPreIncDec::compile(AsmGenerator& context)
{
    AsmBlock* block = new AsmBlock();

    // We have to compile and reference, but the reference function allready compiles
    // and evaluates, thus we use the reference not the value.
    AsmBlock* reference = this->expr.reference(context);
    *block <<   *reference;
    delete reference;

    *block <<   "   SET B, A" << std::endl;

    // Now do the appropriate operation.
    switch (this->op)
    {
        case INCREMENT:
            *block << *(this->m_exprType->inc(context, 'B'));
            break;

        case DECREMENT:
            *block << *(this->m_exprType->dec(context, 'B'));
            break;

        default:
            throw new CompilerException(this->line, this->file, "Unknown Pre-Increase-Decrease operation requested.");
    }

    // return value in A
    *block << *(this->m_exprType->loadFromRef(context, 'B', 'A'));

    return block;
}

AsmBlock* NPreIncDec::reference(AsmGenerator& context)
{
    throw new CompilerException(this->line, this->file, "Unable to get reference to the result of an Pre-Increment.");
}

void NPreIncDec::analyse(AsmGenerator& context, bool reference)
{
    if (reference)
    {
        context.errorList.addError(this->line, this->file, ERR_CC_CANNOT_REFERENCE, " result of an pre-increment operation");
        return;
    }
    
    // analyse expr
    this->expr.analyse(context, true);
    
    // get type
    this->m_exprType = this->expr.getExpressionType(context);
    
    // Type checking
    if ((!this->m_exprType->isPointer()) && (!this->m_exprType->isBasicType()))
    {
        context.errorList.addError(this->line, this->file, ERR_CC_INVALID_INC_DEC_OPERAND, this->m_exprType->getName());
        return;
    }
    
    // check whether this tries to modify a const variable
    if (this->m_exprType->isConst())
    {
        context.errorList.addError(this->line, this->file, ERR_CC_INC_DEC_CONST);
        return;
    }
}

IType* NPreIncDec::getExpressionType(AsmGenerator& context)
{
    // An assignment has the type of it's LHS.
    return this->expr.getExpressionType(context);
}
