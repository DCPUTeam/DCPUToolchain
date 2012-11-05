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
private:
    uint16_t m_globalDataSize;
    bool m_hasMain;
    
public:
    DeclarationList definitions;
    NDeclarations() : Node("declarations"), m_globalDataSize(0), m_hasMain(false) { }
    virtual AsmBlock* compile(AsmGenerator& context);
    virtual AsmBlock* reference(AsmGenerator& context);
    virtual void analyse(AsmGenerator& context, bool reference);
};

#endif
