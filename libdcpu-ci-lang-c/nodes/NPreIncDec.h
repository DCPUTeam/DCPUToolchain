/**

    File:       NPreIncDec.h

    Project:    DCPU-16 Tools
    Component:  LibDCPU-ci-lang-c

    Authors:    Patrick Flick

    Description:    Declares the NPreIncDec AST class.

**/

#ifndef __DCPU_COMP_NODES_PREINCDEC_H
#define __DCPU_COMP_NODES_PREINCDEC_H

#include "NExpression.h"

class NPreIncDec : public NExpression
{
public:
    NExpression& expr;
    int op;
    NPreIncDec(int op, NExpression& expr) :
        expr(expr), op(op), NExpression("preincdec") { }
    virtual AsmBlock* compile(AsmGenerator& context);
    virtual AsmBlock* reference(AsmGenerator& context);
    virtual IType* getExpressionType(AsmGenerator& context);
};

#endif
