/**

    File:           NEmptyStatement.h

    Project:        DCPU-16 Tools
    Component:      LibDCPU-ci-lang-c

    Authors:        James Rhodes, Patrick Flick

    Description:    Declares the NEmptyStatement AST class.

**/

#ifndef __DCPU_COMP_NODES_EMPTY_STATEMENT_H
#define __DCPU_COMP_NODES_EMPTY_STATEMENT_H

#include "NStatement.h"

class NEmptyStatement : public NStatement
{
public:
    NEmptyStatement() : NStatement("empty") { }
    virtual AsmBlock* compile(AsmGenerator& context);
    virtual AsmBlock* reference(AsmGenerator& context);
    virtual void analyse(AsmGenerator& context, bool reference);
};

#endif
