/**

    File:           SymbolTableScope.cpp

    Project:        DCPU-16 Tools
    Component:      LibDCPU-ci-lang

    Authors:        Patrick Flick

    Description:    Defines the SymbolTableScope class.

**/

#include "SymbolTableScope.h"
#include <map>
#include <utility>
#include <iostream>

bool SymbolTableScope::isGlobalScope()
{
    return this->prevScope == NULL;
}

bool SymbolTableScope::contains(std::string name)
{
    if (map.find(name) == map.end())
        return false;
    else
        return true;
}

bool SymbolTableScope::containsRec(std::string name)
{
    if (map.find(name) == map.end())
    {
        if (prevScope == NULL)
            return false;
        else
            return prevScope->containsRec(name);
    }
    else
        return true;
}


uint16_t SymbolTableScope::getParameterStackSize()
{
    return this->m_parameterStackSize;
}

uint16_t SymbolTableScope::getLocalStackSize()
{
    return this->m_localStackSize;
}

uint16_t SymbolTableScope::getGlobalsSize()
{
    return this->m_globalsSize;
}

bool SymbolTableScope::find(std::string name, SymbolObject& result)
{
    std::map<std::string, SymbolObject>::iterator it = this->map.find(name);
    if (it == this->map.end())
    {
        if (prevScope == NULL)
            return false;
        else
            return prevScope->find(name, result);
    }
    else
    {
        result = it->second;
        return true;
    }
}

void SymbolTableScope::insert(std::string name, DeclType declType, IType* type, IDeclaration* decl, ObjectPosition pos)
{
    uint16_t offset = 0;
    // only variables need space (i.e. an offset)
    if (declType == VARIABLE_DECL)
    {
        if (pos == LOCAL_STACK)
        {
            offset = this->m_localStackSize;
            this->m_localStackSize += type->getWordSize(this->m_Generator);
        }
        else if (pos == PARAMETER_STACK)
        {
            offset = this->m_parameterStackSize;
            this->m_parameterStackSize += type->getWordSize(this->m_Generator);
        }
        else if (pos == GLOBAL)
        {
            offset = this->m_globalsSize;
            this->m_globalsSize += type->getWordSize(this->m_Generator);
        }
    }
    this->map.insert(std::pair<std::string, SymbolObject>(name, SymbolObject(name, declType, type, decl, pos, offset)));
}



void SymbolTableScope::endInsert()
{
    // TODO ??
}
