/**

    File:       NCaseStatement.h

    Project:    DCPU-16 Tools
    Component:  LibDCPU-ci-lang-c

    Authors:    Patrick Flick

    Description:    Declares the NCaseStatement AST class.

**/

#ifndef __DCPU_COMP_NODES_CASE_STATEMENT_H
#define __DCPU_COMP_NODES_CASE_STATEMENT_H

#include "NStatement.h"
#include "NExpression.h"

class NCaseStatement : public NStatement
{
private:
    std::string m_constantLiteral;
    std::string m_label;

public:
    NCaseStatement(std::string value);
    NCaseStatement(int value);
    virtual AsmBlock* compile(AsmGenerator& context);
    virtual AsmBlock* reference(AsmGenerator& context);

    std::string getConstantLiteral();
    void setLabelPrefix(std::string prefix);
};

#endif
