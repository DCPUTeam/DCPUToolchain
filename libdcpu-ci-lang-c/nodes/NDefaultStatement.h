/**

    File:       NDefaultStatement.h

    Project:    DCPU-16 Tools
    Component:  LibDCPU-ci-lang-c

    Authors:    Patrick Flick

    Description:    Declares the NDefaultStatement AST class.

**/

#ifndef __DCPU_COMP_NODES_DEFAULT_STATEMENT_H
#define __DCPU_COMP_NODES_DEFAULT_STATEMENT_H

#include "NStatement.h"
#include "NExpression.h"

class NDefaultStatement : public NStatement
{
private:
    std::string m_label;

public:
    NDefaultStatement() : NStatement("default") {};
    virtual AsmBlock* compile(AsmGenerator& context);
    virtual AsmBlock* reference(AsmGenerator& context);

    void setLabelPrefix(std::string prefix);
};

#endif
