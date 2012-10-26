/**

    File:       NForStatement.h

    Project:    DCPU-16 Tools
    Component:  LibDCPU-ci-lang-c

    Authors:    James Rhodes

    Description:    Declares the NForStatement AST class.

**/

#ifndef __DCPU_COMP_NODES_FOR_STATEMENT_H
#define __DCPU_COMP_NODES_FOR_STATEMENT_H

#include "NStatement.h"
#include "NExpression.h"
#include "NBlock.h"

class NForStatement : public NStatement
{
public:
    NExpression& initEval;
    NExpression& checkEval;
    NExpression& loopEval;
    NBlock& expr;
    NForStatement(NExpression& initEval, NExpression& checkEval, NExpression& loopEval, NBlock& expr) :
        initEval(initEval), checkEval(checkEval), loopEval(loopEval), expr(expr), NStatement("for") { }
    virtual AsmBlock* compile(AsmGenerator& context);
    virtual AsmBlock* reference(AsmGenerator& context);
};

#endif
