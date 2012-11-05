/**

    File:           NIntegerLiteral.h

    Project:        DCPU-16 Tools
    Component:      LibDCPU-ci-lang-c

    Authors:        James Rhodes, Patrick Flick

    Description:    Declares the NIntegerLiteral AST class.

**/

#ifndef __DCPU_COMP_NODES_INTEGERLITERAL_H
#define __DCPU_COMP_NODES_INTEGERLITERAL_H

#include "NExpression.h"
#include <nodes/IType.h>

class NIntegerLiteral : public NExpression
{
protected:
    NIntegerLiteral(std::string name) : value(0), NExpression(name) { } // for NGetSizeFunction.
public:
    long long value;
    NIntegerLiteral(long long value) : value(value), NExpression("integer") { }
    virtual AsmBlock* compile(AsmGenerator& context);
    virtual AsmBlock* reference(AsmGenerator& context);
    virtual void analyse(AsmGenerator& context, bool reference);
    virtual IType* getExpressionType(AsmGenerator& context);
};

#endif
