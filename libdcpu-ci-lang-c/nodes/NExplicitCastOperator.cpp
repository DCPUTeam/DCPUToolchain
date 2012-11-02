/**

    File:           NExplicitCastOperator.cpp

    Project:        DCPU-16 Tools
    Component:      LibDCPU-ci-lang-c

    Authors:        Patrick Flick

    Description:    Defines the NExplicitCastOperator AST class.

**/

// Prevents importing all of the class dependencies in
// the parser header so that we only get the token values.
#define YYSTYPE int

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "parser.hpp"
#include "NExplicitCastOperator.h"

AsmBlock* NExplicitCastOperator::compile(AsmGenerator& context)
{
    AsmBlock* block = new AsmBlock();

    // Add file and line information.
    *block << this->getFileAndLineState();

        // get rhs type
    IType* rhsType = this->rhs.getExpressionType(context);

    // cast to rhs to lhs type
    if (rhsType->explicitCastable(context, castType))
    {
        // When an expression is evaluated, the result goes into the A register.
        *block << *(this->rhs.compile(context));
        *block << *(rhsType->explicitCast(context, castType, 'A'));
    }
    else
    {
        throw new CompilerException(this->line, this->file,
                                    "Unable to explicit cast '" + rhsType->getName()
                                    + "' to '" + castType->getName() + "'");
    }

    return block;
}

AsmBlock* NExplicitCastOperator::reference(AsmGenerator& context)
{
    throw new CompilerException(this->line, this->file, "Unable to get reference to the result of an explicit cast operator.");
}

IType* NExplicitCastOperator::getExpressionType(AsmGenerator& context)
{
    // A unary operator has the type of it's expression.
    return this->castType;
}
