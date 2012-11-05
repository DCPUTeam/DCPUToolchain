/**

    File:           SymbolTableScope.h

    Project:        DCPU-16 Tools
    Component:      LibDCPU-ci-lang

    Authors:        Patrick Flick

    Description:    Declares the SymbolTableScope class.

**/

#ifndef __DCPU_LIBDCC_SYMBOLTABLESCOPE_H
#define __DCPU_LIBDCC_SYMBOLTABLESCOPE_H

class AsmGenerator;

#include <string>
#include <map>
#include "nodes/IDeclaration.h"
#include "SymbolTypes.h"
#include "SymbolObject.h"
#include "AsmGenerator.h"
#include "nodes/IType.h"

class SymbolTableScope
{
    // TODO put outside:
    
private:
    AsmGenerator& m_Generator;
    std::map<std::string, SymbolObject> map;
    uint16_t m_localStackSize;
    uint16_t m_parameterStackSize;
    uint16_t m_globalsSize;
public:
    SymbolTableScope* prevScope;

    bool doneInsert;
public:
    SymbolTableScope(AsmGenerator& context) :
            m_Generator(context),
            map(std::map<std::string, SymbolObject>()),
            m_localStackSize(0),
            m_parameterStackSize(0),
            m_globalsSize(0),
            prevScope(NULL),
            doneInsert(false) {}

    SymbolTableScope(SymbolTableScope* prev, AsmGenerator& context) :
            m_Generator(context),
            map(std::map<std::string, SymbolObject>()),
            m_localStackSize(0),
            m_parameterStackSize(0),
            m_globalsSize(0),
            prevScope(prev),
            doneInsert(false) {}
    
    bool isGlobalScope();
    
    bool contains(std::string name);
    bool containsRec(std::string name);
    
    uint16_t getParameterStackSize();
    uint16_t getLocalStackSize();
    uint16_t getGlobalsSize();
    
    // finds any reference
    bool find(std::string name, SymbolObject& result);
    
    void insert(std::string name, DeclType declType, IType* type, IDeclaration* decl, ObjectPosition pos);
    void endInsert();
};

#endif
