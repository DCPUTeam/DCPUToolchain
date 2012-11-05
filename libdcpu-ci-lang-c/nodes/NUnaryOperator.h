/**

    File:           NUnaryOperator.h

    Project:        DCPU-16 Tools
    Component:      LibDCPU-ci-lang-c

    Authors:        Patrick Flick

    Description:    Declares the NUnaryOperator AST class.

**/

#ifndef __DCPU_COMP_NODES_UNARY_OPERATOR_H
#define __DCPU_COMP_NODES_UNARY_OPERATOR_H

#include "NExpression.h"

class NUnaryOperator : public NExpression
{
private:
    IType* m_rhsType;
public:
    NExpression& rhs;
    int op;
    NUnaryOperator(int op, NExpression& rhs) :
        op(op), rhs(rhs), NExpression("unaryop") { }
    virtual AsmBlock* compile(AsmGenerator& context);
    virtual AsmBlock* reference(AsmGenerator& context);
    virtual void analyse(AsmGenerator& context, bool reference);
    virtual IType* getExpressionType(AsmGenerator& context);
};

#endif
