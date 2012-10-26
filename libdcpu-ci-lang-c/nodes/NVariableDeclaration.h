/**

    File:       NVariableDeclaration.h

    Project:    DCPU-16 Tools
    Component:  LibDCPU-ci-lang-c

    Authors:    James Rhodes

    Description:    Declares the NVariableDeclaration AST class.

**/

#ifndef __DCPU_COMP_NODES_VARIABLE_DECLARATION_H
#define __DCPU_COMP_NODES_VARIABLE_DECLARATION_H

#include "NDeclaration.h"
#include "NIdentifier.h"
#include "NExpression.h"
#include "NType.h"

class NVariableDeclaration : public NDeclaration
{
public:
    IType* type;
    NIdentifier& id;
    NExpression* initExpr;
    NVariableDeclaration(IType* type, NIdentifier& id) :
        type(type), id(id), initExpr(NULL), NDeclaration("variable") { }
    NVariableDeclaration(IType* type, NIdentifier& id, NExpression* initExpr) :
        type(type), id(id), initExpr(initExpr), NDeclaration("variable") { }
    virtual AsmBlock* compile(AsmGenerator& context);
    virtual AsmBlock* reference(AsmGenerator& context);
};

#endif
