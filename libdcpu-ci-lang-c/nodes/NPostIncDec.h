/**

    File:           NPostIncDec.h

    Project:        DCPU-16 Tools
    Component:      LibDCPU-ci-lang-c

    Authors:        Patrick Flick

    Description:    Declares the NPostIncDec AST class.

**/

#ifndef __DCPU_COMP_NODES_POSTINCDEC_H
#define __DCPU_COMP_NODES_POSTINCDEC_H

#include "NExpression.h"

class NPostIncDec : public NExpression
{
private:
    IType* m_exprType;
public:
    NExpression& expr;
    int op;
    NPostIncDec(NExpression& expr, int op) :
        expr(expr), op(op), NExpression("postincdec") { }
    virtual AsmBlock* compile(AsmGenerator& context);
    virtual AsmBlock* reference(AsmGenerator& context);
    virtual void analyse(AsmGenerator& context, bool reference);
    virtual IType* getExpressionType(AsmGenerator& context);
};

#endif
