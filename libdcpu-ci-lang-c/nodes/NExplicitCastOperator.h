/**

    File:       NExplicitCastOperator.h

    Project:    DCPU-16 Tools
    Component:  LibDCPU-ci-lang-c

    Authors:    Patrick Flick

    Description:    Declares the NExplicitCastOperator AST class.

**/

#ifndef __DCPU_COMP_NODES_EXPLICITCAST_OPERATOR_H
#define __DCPU_COMP_NODES_EXPLICITCAST_OPERATOR_H

#include "NExpression.h"

class NExplicitCastOperator : public NExpression
{
public:
    NExpression& rhs;
    IType* castType;
    NExplicitCastOperator(IType* castType, NExpression& rhs) :
        castType(castType), rhs(rhs), NExpression("explicitcastop") { }
    virtual AsmBlock* compile(AsmGenerator& context);
    virtual AsmBlock* reference(AsmGenerator& context);
    virtual IType* getExpressionType(AsmGenerator& context);
};

#endif
