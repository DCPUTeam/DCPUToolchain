/**

    File:           NPostIncDec.cpp

    Project:        DCPU-16 Tools
    Component:      LibDCPU-ci-lang-c

    Authors:        Patrick Flick

    Description:    Defines the NPostIncDec AST class.

**/

// Prevents importing all of the class dependencies in
// the parser header so that we only get the token values.
#define YYSTYPE int

#include <AsmGenerator.h>
#include <CompilerException.h>
#include <nodes/IType.h>
#include "parser.hpp"
#include "NPostIncDec.h"
#include <derr.defs.h>

AsmBlock* NPostIncDec::compile(AsmGenerator& context)
{
    AsmBlock* block = new AsmBlock();

    // We have to compile and reference, but the reference function allready compiles
    // and evaluates, thus we use the reference not the value.
    AsmBlock* reference = this->expr.reference(context);
    *block <<   *reference;
    delete reference;

    *block <<   "   SET B, A" << std::endl;
    // return old value in A
    *block << *(this->m_exprType->loadFromRef(context, 'B', 'A'));

    // increment/decrement
    switch (this->op)
    {
        case INCREMENT:
            *block << *(this->m_exprType->inc(context, 'B'));
            break;

        case DECREMENT:
            *block << *(this->m_exprType->dec(context, 'B'));
            break;

        default:
            throw new CompilerException(this->line, this->file, "Unknown Post-Increase-Decrease operation requested.");
    }

    return block;
}

AsmBlock* NPostIncDec::reference(AsmGenerator& context)
{
    throw new CompilerException(this->line, this->file, "Unable to get reference to the result of an Post-Increment.");
}

void NPostIncDec::analyse(AsmGenerator& context, bool reference)
{
    if (reference)
    {
        context.errorList.addError(this->line, this->file, ERR_CC_CANNOT_REFERENCE, " result of an post-increment operation");
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

IType* NPostIncDec::getExpressionType(AsmGenerator& context)
{
    // An assignment has the type of it's LHS.
    return this->expr.getExpressionType(context);
}
