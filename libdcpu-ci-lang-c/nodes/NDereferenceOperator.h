/**

    File:       NDereferenceOperator.h

    Project:    DCPU-16 Tools
    Component:  LibDCPU-ci-lang-c

    Authors:    James Rhodes

    Description:    Declares the NDereferenceOperator AST class.

**/

#ifndef __DCPU_COMP_NODES_DEREFERENCE_OPERATOR_H
#define __DCPU_COMP_NODES_DEREFERENCE_OPERATOR_H

#include "NExpression.h"

class NDereferenceOperator : public NExpression
{
private:
    IType* m_baseType;
public:
    NExpression& expr;
    NDereferenceOperator(NExpression& expr) : expr(expr), NExpression("dereference") { }
    virtual AsmBlock* compile(AsmGenerator& context);
    virtual AsmBlock* reference(AsmGenerator& context);
    virtual void analyse(AsmGenerator& context, bool reference);
    virtual IType* getExpressionType(AsmGenerator& context);
};

#endif
