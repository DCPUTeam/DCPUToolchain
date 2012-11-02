/**

    File:           NReturnStatement.h

    Project:        DCPU-16 Tools
    Component:      LibDCPU-ci-lang-c

    Authors:        James Rhodes
                    Patrick Flick

    Description:    Declares the NReturnStatement AST class.

**/

#ifndef __DCPU_COMP_NODES_RETURN_STATEMENT_H
#define __DCPU_COMP_NODES_RETURN_STATEMENT_H

#include "NStatement.h"
#include "NExpression.h"

class NReturnStatement : public NStatement
{
public:
    NExpression* result;
    NReturnStatement(NExpression* result) :
        result(result), NStatement("return") { }
    virtual AsmBlock* compile(AsmGenerator& context);
    virtual AsmBlock* reference(AsmGenerator& context);
};

#endif
