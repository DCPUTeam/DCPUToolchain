/**

    File:       NPreIncDec.cpp

    Project:    DCPU-16 Tools
    Component:  LibDCPU-ci-lang-c

    Authors:    Patrick Flick

    Description:    Defines the NPreIncDec AST class.

**/

// Prevents importing all of the class dependencies in
// the parser header so that we only get the token values.
#define YYSTYPE int

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "parser.hpp"
#include "NPreIncDec.h"

AsmBlock* NPreIncDec::compile(AsmGenerator& context)
{
    AsmBlock* block = new AsmBlock();

    // We have to compile and reference, but the reference function allready compiles
    // and evaluates, thus we use the reference not the value.
    AsmBlock* reference = this->expr.reference(context);
    *block <<   *reference;
    delete reference;

    // get type
    IType* exprType = this->expr.getExpressionType(context);

    // Type checking
    if ((!exprType->isPointer()) && (!exprType->isBasicType()))
    {
        throw new CompilerException(this->line, this->file,
                                    "Invalid operand to pre increase/decrease operation. (have '"
                                    + exprType->getName() + "')");
    }


    *block <<   "   SET B, A" << std::endl;


    // Now do the appropriate operation.
    switch (this->op)
    {
        case INCREMENT:
            *block << *(exprType->inc(context, 'B'));
            break;

        case DECREMENT:
            *block << *(exprType->dec(context, 'B'));
            break;

        default:
            throw new CompilerException(this->line, this->file, "Unknown Pre-Increase-Decrease operation requested.");
    }

    // return value in A
    *block << *(exprType->loadFromRef(context, 'B', 'A'));

    return block;
}

AsmBlock* NPreIncDec::reference(AsmGenerator& context)
{
    throw new CompilerException(this->line, this->file, "Unable to get reference to the result of an Pre-Increment.");
}

IType* NPreIncDec::getExpressionType(AsmGenerator& context)
{
    // An assignment has the type of it's LHS.
    return this->expr.getExpressionType(context);
}
