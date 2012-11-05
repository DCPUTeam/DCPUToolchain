/**

    File:           NSwitchStatement.h

    Project:        DCPU-16 Tools
    Component:      LibDCPU-ci-lang-c

    Authors:        Patrick Flick

    Description:    Declares the NSwitchStatement AST class.

**/

#ifndef __DCPU_COMP_NODES_SWITCH_STATEMENT_H
#define __DCPU_COMP_NODES_SWITCH_STATEMENT_H

#include "NStatement.h"
#include "NExpression.h"
#include "NBlock.h"
#include "NDefaultStatement.h"
#include <vector>
#include <string>

class NSwitchStatement : public NStatement
{
private:
    NDefaultStatement* m_defaultStmt;
    std::string m_endlbl;
    std::string m_caselbl;
    std::vector<std::string> m_cases;
public:
    NExpression& eval;
    NBlock& innerBlock;
    NSwitchStatement(NExpression& eval, NBlock& innerBlock) :
        eval(eval), innerBlock(innerBlock), NStatement("switch") { }
    virtual AsmBlock* compile(AsmGenerator& context);
    virtual AsmBlock* reference(AsmGenerator& context);
    virtual void analyse(AsmGenerator& context, bool reference);
};

#endif
