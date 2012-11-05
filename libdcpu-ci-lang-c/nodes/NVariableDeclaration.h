/**

    File:           NVariableDeclaration.h

    Project:        DCPU-16 Tools
    Component:      LibDCPU-ci-lang-c

    Authors:        James Rhodes, Patrick Flick

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
private:
    TypePosition m_varPos;
    IType* m_exprType;
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
    virtual void analyse(AsmGenerator& context, bool reference);
    
    virtual void insertIntoScope(AsmGenerator& context, SymbolTableScope& scope, ObjectPosition position);
};

#endif
