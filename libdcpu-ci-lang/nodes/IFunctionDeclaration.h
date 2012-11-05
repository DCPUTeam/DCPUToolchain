/**

    File:       IFunctionDeclaration.h

    Project:    DCPU-16 Tools
    Component:  LibDCPU-CI-Lang

    Authors:    James Rhodes

    Description:    Declares the IFunctionDeclaration AST interface.

**/

#ifndef __DCPU_COMP_INTERFACES_FUNCTION_DECLARATION_H
#define __DCPU_COMP_INTERFACES_FUNCTION_DECLARATION_H

//class IDeclaration;

#include "IDeclaration.h"
#include "../StackMap.h"


class IFunctionDeclaration : public IDeclaration
{
public:
    virtual IType* getPointerType() = 0;
};

#endif
