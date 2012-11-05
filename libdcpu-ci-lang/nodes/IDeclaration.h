/**

    File:           IDeclaration.h

    Project:        DCPU-16 Tools
    Component:      LibDCPU-ci-lang

    Authors:        Patrick Flick

    Description:    Declares the IDeclaration AST interface.

**/

#ifndef __DCPU_COMP_INTERFACES_DECLARATION_H
#define __DCPU_COMP_INTERFACES_DECLARATION_H


class SymbolObject;
class SymbolTableScope;
class AsmGenerator;

#include "../SymbolTypes.h"

class IDeclaration
{
public:
    virtual void insertIntoScope(AsmGenerator& context, SymbolTableScope& scope, ObjectPosition position) = 0;
};


#include "../AsmGenerator.h"


#include "../SymbolObject.h"
#include "../SymbolTableScope.h"



#endif
