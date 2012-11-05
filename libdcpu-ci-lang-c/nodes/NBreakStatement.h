/**

    File:       NBreakStatement.h

    Project:    DCPU-16 Tools
    Component:  LibDCPU-ci-lang-c

    Authors:    Patrick Flick

    Description:    Declares the NBreakStatement AST class.

**/

#ifndef __DCPU_COMP_NODES_BREAK_STATEMENT_H
#define __DCPU_COMP_NODES_BREAK_STATEMENT_H

#include "NStatement.h"
#include "NExpression.h"

class NBreakStatement : public NStatement
{
private:
    std::string m_breakLabel;

public:
    NBreakStatement() :
        NStatement("break") { }
    virtual AsmBlock* compile(AsmGenerator& context);
    virtual void analyse(AsmGenerator& context, bool reference);
    virtual AsmBlock* reference(AsmGenerator& context);
};

#endif
