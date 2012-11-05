/**

    File:           SymbolTable.h

    Project:        DCPU-16 Tools
    Component:      LibDCPU-ci-lang

    Authors:        Patrick Flick

    Description:    Declares the SymbolTable class.

**/

#ifndef __DCPU_LIBDCC_SYMBOLTABLE_H
#define __DCPU_LIBDCC_SYMBOLTABLE_H

class AsmGenerator;

#include <string>
#include "AsmGenerator.h"
#include "SymbolObject.h"
#include "SymbolTableScope.h"
#include "nodes/IDeclaration.h"
#include "nodes/IFunctionDeclaration.h"
#include "nodes/IType.h"
#include "TypePosition.h"

class SymbolTable
{
private:
    SymbolTableScope* m_globalScope;
    SymbolTableScope* m_currentScope;
    
public:
    AsmGenerator& m_Generator;
    

    
    SymbolTable(AsmGenerator& context);
    
    SymbolTableScope* beginNewScope();
    void beginScope(SymbolTableScope* newScope);
    void endScope();
    
    bool contains(std::string name);
    bool containsRec(std::string name);
    bool find(std::string name, SymbolObject& result);
   // void insert(std::string name, IType* type, StackFramePosition stackPosition);
    
    SymbolTableScope& getCurrentScope();
    SymbolTableScope& getGlobalScope();
    bool isGlobalCurScope();
    
    TypePosition getPositionOfVariable(std::string name);
    TypePosition getPositionOfVariable(std::string name, bool previousStackFrame);
    IType* getTypeOfVariable(std::string name);
    IFunctionDeclaration* getFunction(std::string name);
    IDeclaration* getStructDeclaration(std::string name);
    //uint16_t getSizeOfVariable(std::string name);
};

#endif
