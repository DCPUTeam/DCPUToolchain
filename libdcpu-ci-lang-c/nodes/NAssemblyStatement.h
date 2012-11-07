/**

    File:           NAssemblyStatement.h

    Project:        DCPU-16 Tools
    Component:      LibDCPU-ci-lang-c

    Authors:        James Rhodes, Patrick Flick

    Description:    Declares the NAssemblyStatement AST class.

**/

#ifndef __DCPU_COMP_NODES_ASSEMBLY_STATEMENT_H
#define __DCPU_COMP_NODES_ASSEMBLY_STATEMENT_H

#include <string>
#include "NStatement.h"
#include "NExpression.h"

class NAssemblyStatement : public NStatement
{
private:
    std::string m_resultAsm;

public:
    std::string asmcode;
    NAssemblyStatement(const std::string& asmcode) :
        asmcode(asmcode), NStatement("assembly") { }
    virtual AsmBlock* compile(AsmGenerator& context);
    virtual AsmBlock* reference(AsmGenerator& context);
    virtual void analyse(AsmGenerator& context, bool reference);
};

#endif
