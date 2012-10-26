/**

    File:       NArrayDeclaration.h

    Project:    DCPU-16 Tools
    Component:  LibDCPU-ci-lang-c

    Authors:    Patrick Flick

    Description:    Declares the NArrayDeclaration AST class.

**/

#ifndef __DCPU_COMP_NODES_ARRAY_DECLARATION_H
#define __DCPU_COMP_NODES_ARRAY_DECLARATION_H

#include "NDeclaration.h"
#include "NIdentifier.h"
#include "NExpression.h"
#include "NType.h"
#include "TPointer16.h"
#include "Lists.h"

class NArrayDeclaration : public NDeclaration
{
    DimensionsList* m_dims;


    uint16_t m_numElements;

    IType* m_baseType;
    IType* m_pointerType;
    IType* m_memAreaType;

public:
    ExpressionList* m_initExprs;
    NIdentifier& id;

    IType* getPointerType();
    IType* getMemAreaType();
    //IType* type;

    NArrayDeclaration(IType* type, NIdentifier& id, DimensionsList* dims, ExpressionList* initExprList);
    virtual AsmBlock* compile(AsmGenerator& context);
    virtual AsmBlock* initPointerTable(AsmGenerator& context, char arrayPointerAdr, char arrayMemAreaAdr);
    virtual AsmBlock* reference(AsmGenerator& context);
};

#endif
