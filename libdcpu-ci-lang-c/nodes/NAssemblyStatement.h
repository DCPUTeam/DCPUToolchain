/**

    File:       NAssemblyStatement.h

    Project:    DCPU-16 Tools
    Component:  LibDCPU-ci-lang-c

    Authors:    James Rhodes

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
    void findreplace(std::string& str, const std::string find, const std::string replace);
    void finderrorglob(std::string& str, const std::string find);
    void finderrorref(std::string& str, const std::string find);

public:
    std::string asmcode;
    NAssemblyStatement(const std::string& asmcode) :
        asmcode(asmcode), NStatement("assembly") { }
    virtual AsmBlock* compile(AsmGenerator& context);
    virtual AsmBlock* reference(AsmGenerator& context);
    virtual void analyse(AsmGenerator& context, bool reference);
};

#endif
