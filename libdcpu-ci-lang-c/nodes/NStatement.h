/**

    File:       NStatement.h

    Project:    DCPU-16 Tools
    Component:  LibDCPU-ci-lang-c

    Authors:    James Rhodes

    Description:    Declares the NStatement AST class.

**/

#ifndef __DCPU_COMP_NODES_STATEMENT_H
#define __DCPU_COMP_NODES_STATEMENT_H

#include "Node.h"
#include <string>

class NStatement : public Node
{
protected:
    NStatement(std::string type) : Node("statement-" + type) { }
};

#endif
