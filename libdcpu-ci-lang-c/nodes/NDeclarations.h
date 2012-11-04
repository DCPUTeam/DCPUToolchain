/**

    File:       NDeclarations.h

    Project:    DCPU-16 Tools
    Component:  LibDCPU-ci-lang-c

    Authors:    James Rhodes

    Description:    Declares the NDeclarations AST class.

**/

#ifndef __DCPU_COMP_NODES_DECLARATIONS_H
#define __DCPU_COMP_NODES_DECLARATIONS_H

#include <nodes/IDeclarations.h>
#include "Node.h"
#include "Lists.h"

class NDeclarations : public Node, public IDeclarations
{
public:
    DeclarationList definitions;
    NDeclarations() : Node("declarations") { }
    virtual AsmBlock* compile(AsmGenerator& context);
    virtual AsmBlock* reference(AsmGenerator& context);
    //virtual void analyse(AsmGenerator& context, bool reference);
    //virtual IFunctionDeclaration* getFunction(std::string name);
};

#endif
