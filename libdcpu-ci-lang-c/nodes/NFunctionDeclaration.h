/**

    File:       NFunctionDeclaration.h

    Project:    DCPU-16 Tools
    Component:  LibDCPU-ci-lang-c

    Authors:    James Rhodes

    Description:    Declares the NFunctionDeclaration AST class.

**/

#ifndef __DCPU_COMP_NODES_FUNCTION_DECLARATION_H
#define __DCPU_COMP_NODES_FUNCTION_DECLARATION_H

#include <nodes/IFunctionDeclaration.h>
#include "NDeclaration.h"
#include "NIdentifier.h"
#include "NBlock.h"
#include "NType.h"
#include "NFunctionPointerType.h"

class NFunctionDeclaration : public NDeclaration, public NFunctionSignature, public IFunctionDeclaration
{
private:
    bool m_isDeclaration;

public:
    const NIdentifier& id;
    NBlock* block;
    NFunctionPointerType* pointerType;
    
    SymbolTableScope* functionScope;
    
    
    NFunctionDeclaration(const IType* type, const NIdentifier& id, const VariableList& arguments, NBlock* block, bool varArgs);
    ~NFunctionDeclaration();
    
    virtual AsmBlock* compile(AsmGenerator& context);
    virtual AsmBlock* reference(AsmGenerator& context);
    virtual void analyse(AsmGenerator& context, bool reference);

    virtual IType* getPointerType();
    virtual void insertIntoScope(AsmGenerator& context, SymbolTableScope& scope, ObjectPosition position);

};

#endif
