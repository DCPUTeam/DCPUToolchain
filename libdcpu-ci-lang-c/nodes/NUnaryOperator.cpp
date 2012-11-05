/**

    File:           NUnaryOperator.cpp

    Project:        DCPU-16 Tools
    Component:      LibDCPU-ci-lang-c

    Authors:        Patrick Flick

    Description:    Defines the NUnaryOperator AST class.

**/

// Prevents importing all of the class dependencies in
// the parser header so that we only get the token values.
#define YYSTYPE int

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "parser.hpp"
#include "NUnaryOperator.h"
#include <derr.defs.h>

AsmBlock* NUnaryOperator::compile(AsmGenerator& context)
{
    AsmBlock* block = new AsmBlock();

    // Add file and line information.
    *block << this->getFileAndLineState();

    // When an expression is evaluated, the result goes into the A register.
    AsmBlock* rhs = this->rhs.compile(context);

    // Move the value into A
    *block <<   *rhs;
    delete rhs;

    // Now do the appropriate operation.
    AsmBlock* compiledOp;
    switch (this->op)
    {
        case ADD:
            /* TODO integer promotion */
            compiledOp = this->m_rhsType->plus(context, 'A');
            break;

            /* unary negative:  "A = -A" */
        case SUBTRACT:
            // A = 0 - A
            compiledOp = this->m_rhsType->minus(context, 'A');
            break;

            /* unary bitwise negate:  "A = ~A" */
        case BITWISE_NEGATE:
            compiledOp = this->m_rhsType->bnot(context, 'A');
            break;
            /* boolean negate: A = !A  */
        case NEGATE:
            compiledOp = this->m_rhsType->lnot(context, 'A');
            break;

        default:
            throw new CompilerException(this->line, this->file, "Unknown unary operations requested.");
    }
    *block << *compiledOp;

    return block;
}

AsmBlock* NUnaryOperator::reference(AsmGenerator& context)
{
    throw new CompilerException(this->line, this->file, "Unable to get reference to the result of an unary operator.");
}

void NUnaryOperator::analyse(AsmGenerator& context, bool reference)
{
    if (reference)
    {
        context.errorList.addError(this->line, this->file, ERR_CC_CANNOT_REFERENCE, " an unary operation");
        return;
    }
    
    this->rhs.analyse(context, false);
    
    // get type
    this->m_rhsType = this->rhs.getExpressionType(context);

    if (!this->m_rhsType->isBasicType())
    {
        context.errorList.addError(this->line, this->file, ERR_CC_INVALID_UNARY_OPERAND, this->m_rhsType->getName());
        return;
    }
}

IType* NUnaryOperator::getExpressionType(AsmGenerator& context)
{
    // A unary operator has the type of it's expression.
    return this->rhs.getExpressionType(context);
}
