/**

    File:       NFunctionSignature.h

    Project:    DCPU-16 Tools
    Component:  LibDCPU-ci-lang-c

    Authors:    James Rhodes

    Description:    Declares the NFunctionSignature AST class.

**/

#ifndef __DCPU_COMP_NODES_FUNCTION_SIGNATURE_H
#define __DCPU_COMP_NODES_FUNCTION_SIGNATURE_H

#include <nodes/IFunctionSignature.h>
#include "NType.h"
#include <nodes/IType.h>
#include "Lists.h"

class NFunctionSignature : public IFunctionSignature
{
public:
    const IType* type;
    const VariableList arguments;
    NFunctionSignature(const IType* type, const VariableList& arguments)
        : type(type), arguments(arguments) { };
    std::string getSignature();
    static std::string calculateSignature(const IType* returnType, const VariableList& arguments);
    static std::string calculateSignature(const VariableList& arguments);
    virtual StackMap generateStackMap();
};

#endif
