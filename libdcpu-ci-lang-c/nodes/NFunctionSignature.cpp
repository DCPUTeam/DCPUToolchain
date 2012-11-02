/**

    File:       NFunctionSignature.cpp

    Project:    DCPU-16 Tools
    Component:  LibDCPU-ci-lang-c

    Authors:    James Rhodes

    Description:    Defines the NFunctionSignature AST class.

**/

#include <AsmGenerator.h>
#include "NFunctionSignature.h"
#include "Lists.h"

std::string NFunctionSignature::calculateSignature(const VariableList& arguments, bool varArgs)
{
    std::string sig = "(";
    for (VariableList::const_iterator i = arguments.begin(); i != arguments.end(); i++)
    {
        if (i != arguments.begin())
        {
            sig = sig + ",";
        }
        sig = sig + (*i)->type->getName();
    }
    if (varArgs)
        sig = sig + ",...";
    sig = sig + ")";
    return sig;
}

bool NFunctionSignature::callerSignatureMatching(const ExpressionList& callerArguments)
{
    // FIXME check the argument types correctly
    // FIXME this^ is kind of important! :D
    if (this->varArgs)
        return (callerArguments.size() >= this->arguments.size());
    else
        return (callerArguments.size() == this->arguments.size());
}



std::string NFunctionSignature::calculateSignature(const IType* returnType, const VariableList& arguments)
{
    IType* returnTypePtr = (IType*) returnType;
    std::string name = returnTypePtr->getName();
    std::string argumentsSig = NFunctionSignature::calculateSignature(arguments, false);
    name = name + "-" + argumentsSig;
    return std::string(name);
}


std::string NFunctionSignature::getSignature()
{
    return NFunctionSignature::calculateSignature(this->arguments, this->varArgs);
}

StackMap NFunctionSignature::generateStackMap()
{
    StackMap map;

    // Add stack frame data for arguments.
    for (VariableList::const_iterator i = this->arguments.begin(); i != this->arguments.end(); i++)
    {
        map.insert(map.end(), StackPair((*i)->id.name, (*i)->type));
        // FIXME: Check to make sure arguments do not clash with any
        //    other argument declarations (hint: check the map to
        //    see if it already has a match).
    }

    return map;
}
