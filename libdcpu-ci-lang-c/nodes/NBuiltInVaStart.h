/**

    File:           NBuiltInVaStart.h

    Project:        DCPU-16 Tools
    Component:      LibDCPU-ci-lang-c

    Authors:        Patrick Flick

    Description:    Declares the NMethodCall AST class.

**/

#ifndef __DCPU_COMP_NODES_BUILT_IN_VA_START_H
#define __DCPU_COMP_NODES_BUILT_IN_VA_START_H

class AsmBlock;

#include "NExpression.h"
#include "NIdentifier.h"
#include "Lists.h"

class NBuiltInVaStart : public NExpression
{
public:
    NIdentifier& vaListID;
    NIdentifier& paramNID;
    NBuiltInVaStart(NIdentifier& vaListID, NIdentifier& paramNID) :
        vaListID(vaListID), paramNID(paramNID), NExpression("builtin_va_list") { }
    virtual AsmBlock* compile(AsmGenerator& context);
    virtual AsmBlock* reference(AsmGenerator& context);
    virtual void analyse(AsmGenerator& context, bool reference);
    virtual IType* getExpressionType(AsmGenerator& context);
};

#endif
