/**

    File:           NForStatement.h

    Project:        DCPU-16 Tools
    Component:      LibDCPU-ci-lang-c

    Authors:        James Rhodes, Patrick Flick

    Description:    Declares the NForStatement AST class.

**/

#ifndef __DCPU_COMP_NODES_FOR_STATEMENT_H
#define __DCPU_COMP_NODES_FOR_STATEMENT_H

#include "NStatement.h"
#include "NExpression.h"
#include "NBlock.h"
#include <string>

class NForStatement : public NStatement
{
private:
    std::string m_startlbl;
    std::string m_endlbl;
    std::string m_continuelbl;
public:
    NExpression& initEval;
    NExpression& checkEval;
    NExpression& loopEval;
    NBlock& expr;
    NForStatement(NExpression& initEval, NExpression& checkEval, NExpression& loopEval, NBlock& expr) :
        initEval(initEval), checkEval(checkEval), loopEval(loopEval), expr(expr), NStatement("for") { }
    virtual AsmBlock* compile(AsmGenerator& context);
    virtual AsmBlock* reference(AsmGenerator& context);
    virtual void analyse(AsmGenerator& context, bool reference);
};

#endif
