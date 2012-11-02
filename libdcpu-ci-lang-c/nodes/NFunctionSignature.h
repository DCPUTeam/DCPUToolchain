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
    const bool varArgs;
    NFunctionSignature(const IType* type, const VariableList& arguments, bool varArgs)
        : type(type), arguments(arguments), varArgs(varArgs) { };
    std::string getSignature();
    bool callerSignatureMatching(const ExpressionList& arguments);
    static std::string calculateSignature(const IType* returnType, const VariableList& arguments);
    static std::string calculateSignature(const VariableList& arguments, bool varArgs);
    virtual StackMap generateStackMap();
};

#endif
