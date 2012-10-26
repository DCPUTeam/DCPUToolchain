/**

    File:       NType.h

    Project:    DCPU-16 Tools
    Component:  LibDCPU-ci-lang-c

    Authors:    James Rhodes

    Description:    Declares the NType AST class.

**/

#ifndef __DCPU_COMP_NODES_TYPE_H
#define __DCPU_COMP_NODES_TYPE_H

class NStructureDeclaration;

#include <stdint.h>
#include <nodes/IType.h>
#include "NIdentifier.h"

class NType : public NIdentifier, public IType
{
private:
    std::string _name;

protected:
    NType(std::string classifier, const std::string& name, unsigned int pointerCount, bool isStruct, bool isConstant, bool isSigned) :
        pointerCount(pointerCount), isStruct(isStruct), isConstant(isConstant), isSigned(isSigned), _name(name), NIdentifier(name, "type-" + classifier) { }

public:
    std::string getName() const;
    unsigned int pointerCount;
    bool isStruct;
    bool isConstant;
    bool isSigned;

    NType(const std::string& name, unsigned int pointerCount, bool isStruct, bool isConstant, bool isSigned) :
        pointerCount(pointerCount), isStruct(isStruct), isConstant(isConstant), isSigned(isSigned), _name(name), NIdentifier(name, "type") { }

    size_t getBitSize(AsmGenerator& context);
    virtual uint16_t getWordSize(AsmGenerator& context);
    virtual AsmBlock* compile(AsmGenerator& context);
    virtual AsmBlock* reference(AsmGenerator& context);
    virtual IType* getExpressionType(AsmGenerator& context);

    // Operator overloads.
    bool operator==(const NType& other) const;
    bool operator!=(const NType& other) const;

    // The type that represents types.
    static NType& TypeType;

    // The "void" type.
    static NType& VoidType;
};

#endif
