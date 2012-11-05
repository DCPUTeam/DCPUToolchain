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
#include <derr.defs.h>

AsmBlock* NExplicitCastOperator::compile(AsmGenerator& context)
{
    AsmBlock* block = new AsmBlock();

    // Add file and line information.
    *block << this->getFileAndLineState();

    // cast to rhs to lhs type
    if (this->m_rhsType->explicitCastable(context, castType))
    {
        // When an expression is evaluated, the result goes into the A register.
        *block << *(this->rhs.compile(context));
        *block << *(this->m_rhsType->explicitCast(context, castType, 'A'));
    }

    return block;
}

AsmBlock* NExplicitCastOperator::reference(AsmGenerator& context)
{
    throw new CompilerException(this->line, this->file, "Unable to get reference to the result of an explicit cast operator.");
}

void NExplicitCastOperator::analyse(AsmGenerator& context, bool reference)
{
    if (reference)
    {
        context.errorList.addError(this->line, this->file, ERR_CC_CANNOT_REFERENCE, " an explicit cast operator");
        return;
    }
    
    // analyse rhs node
    rhs.analyse(context, false);
    
    // get rhs type
    this->m_rhsType = this->rhs.getExpressionType(context);

    // cast to rhs to lhs type
    if (!this->m_rhsType->explicitCastable(context, castType))
    {
        context.errorList.addError(this->line, this->file, ERR_CC_CANNOT_EXPLICIT_CAST, "'" + this->m_rhsType->getName()
                                    + "' to '" + castType->getName() + "'");
    }
}

IType* NExplicitCastOperator::getExpressionType(AsmGenerator& context)
{
    // A unary operator has the type of it's expression.
    return this->castType;
}
