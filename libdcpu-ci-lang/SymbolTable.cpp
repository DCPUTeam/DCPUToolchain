/**

    File:           SymbolTable.cpp

    Project:        DCPU-16 Tools
    Component:      LibDCPU-ci-lang

    Authors:        Patrick Flick

    Description:    Defines the SymbolTable class.

**/

#include "SymbolTypes.h"
#include "SymbolTableScope.h"
#include "SymbolTable.h"


SymbolTable::SymbolTable(AsmGenerator& context) : m_Generator(context)
{
    this->m_globalScope = new SymbolTableScope(context);
    this->m_currentScope = this->m_globalScope;
}

SymbolTableScope& SymbolTable::getCurrentScope()
{
    return *(this->m_currentScope);
}

SymbolTableScope& SymbolTable::getGlobalScope()
{
    return *(this->m_globalScope);
}

bool SymbolTable::isGlobalCurScope()
{
    return (this->m_globalScope == this->m_currentScope);
}

SymbolTableScope* SymbolTable::beginNewScope()
{
    SymbolTableScope* tmpScope = this->m_currentScope;
    this->m_currentScope = new SymbolTableScope(tmpScope, this->m_Generator);
    return this->m_currentScope;
}

void SymbolTable::beginScope(SymbolTableScope* newScope)
{
    SymbolTableScope* tmpScope = this->m_currentScope;
    this->m_currentScope = newScope;
    newScope->prevScope = tmpScope;
}

void SymbolTable::endScope()
{
    if (this->m_currentScope->prevScope != NULL)
    {
        SymbolTableScope* tmpScope = this->m_currentScope->prevScope;
        //delete this->m_currentScope;
        this->m_currentScope = tmpScope;
    }
}

bool SymbolTable::contains(std::string name)
{
    return this->m_currentScope->contains(name);
}

bool SymbolTable::containsRec(std::string name)
{
    return this->m_currentScope->containsRec(name);
}

bool SymbolTable::find(std::string name, SymbolObject& result)
{
    return this->m_currentScope->find(name, result);
}
/*
void SymbolTable::insert(std::string name, IType* type, StackFramePosition stackPosition)
{
    this->currentScope->insert(name, type, stackPosition);
}
* */

TypePosition SymbolTable::getPositionOfVariable(std::string name, bool previousStackFrame)
{
    SymbolObject obj;
    if(this->m_currentScope->find(name, obj))
    {
        // FIXME:
        //    use stack-frames up instead of 'previousStackFrame', this way variables mutliple stack frames up can still be found
        // TODO:
        //    implement GLOBAL symbols aswell, to be used in global stack frame
        //    implement recursive and non-recursive versions of 'contains' and 'find'
        //    replace StackMap, StackFrame and all the calls to it in the compiler
        if (obj.objectPos == LOCAL_STACK)
            return TypePosition(true, false, false, previousStackFrame, obj.positionOffset);
        else if (obj.objectPos == PARAMETER_STACK)
            return TypePosition(true, false, true, previousStackFrame, obj.positionOffset);
        else if (obj.objectPos == GLOBAL)
        {
            if (obj.declType == FUNCTION_DECL)
                return TypePosition(true, name);
            else
                return TypePosition(true, true, false, previousStackFrame, obj.positionOffset);
        }
    }

    return TypePosition(false, false, false, false, 0);
}

TypePosition SymbolTable::getPositionOfVariable(std::string name)
{
    return this->getPositionOfVariable(name, false);
}

IType* SymbolTable::getTypeOfVariable(std::string name)
{
    SymbolObject obj;
    if(this->m_currentScope->find(name, obj))
    {
        if (obj.declType == VARIABLE_DECL)
            return obj.type;
        else
            return NULL;
    }
    else
    {
        return NULL;
    }
}

IFunctionDeclaration* SymbolTable::getFunction(std::string name)
{
    SymbolObject obj;
    // search for functions only at global scope:
    if(this->m_globalScope->find(name, obj))
    {
        if (obj.declType == FUNCTION_DECL)
            return (IFunctionDeclaration*) obj.decl;
        else
            return NULL;
    }
    else
    {
        return NULL;
    }
}

IDeclaration* SymbolTable::getStructDeclaration(std::string name)
{
    SymbolObject obj;
    if(this->m_currentScope->find(name, obj))
    {
        if (obj.declType == STRUCT_DECL)
            return obj.decl;
        else
            return NULL;
    }
    else
    {
        return NULL;
    }
}
