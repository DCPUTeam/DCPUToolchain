/**

    File:           NWhileStatement.h

    Project:        DCPU-16 Tools
    Component:      LibDCPU-ci-lang-c

    Authors:        James Rhodes, Patrick Flick

    Description:    Declares the NWhileStatement AST class.

**/

#ifndef __DCPU_COMP_NODES_WHILE_STATEMENT_H
#define __DCPU_COMP_NODES_WHILE_STATEMENT_H

#include "NStatement.h"
#include "NExpression.h"
#include "NBlock.h"

class NWhileStatement : public NStatement
{
private:
    std::string m_startlbl;
    std::string m_endlbl;
public:
    NExpression& eval;
    NBlock& expr;
    NWhileStatement(NExpression& eval, NBlock& expr) :
        eval(eval), expr(expr), NStatement("while") { }
    virtual AsmBlock* compile(AsmGenerator& context);
    virtual AsmBlock* reference(AsmGenerator& context);
    virtual void analyse(AsmGenerator& context, bool reference);
};

#endif
