/**

    File:       NType.cpp

    Project:    DCPU-16 Tools
    Component:  LibDCPU-ci-lang-c

    Authors:    James Rhodes

    Description:    Defines the NType AST class.

**/

#include <stdexcept>
#include <cmath>
#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NType.h"
#include "NDeclarations.h"
#include "NStructureDeclaration.h"
#include "Lists.h"

NType internal_TypeType("<type>", 0, false, true, false);
NType internal_VoidType("void", 0, false, false, false);
NType& NType::TypeType(internal_TypeType);
NType& NType::VoidType(internal_VoidType);


// TODO remove this later, as soon as Type System is implemented
std::string NType::getName() const
{
    // TODO maybe call it by its real name? (e.g. unsigned int, unsigned char, uint16_t)
    return _name;
}


AsmBlock* NType::compile(AsmGenerator& context)
{
    throw new CompilerException(this->line, this->file, "Unable to compile type AST node.");
}

AsmBlock* NType::reference(AsmGenerator& context)
{
    throw new CompilerException(this->line, this->file, "Unable to get reference to the result of a type.");
}

size_t NType::getBitSize(AsmGenerator& context)
{
    if (this->pointerCount > 0)
        return 16;
    else
    {
        if (this->name == "void")       return 0; // for void pointers
        else if (this->name == "char")      return 16; // 8
        else if (this->name == "byte")      return 16; // 8
        else if (this->name == "short")     return 16;
        else if (this->name == "int")       return 16;
        else if (this->name == "long")      return 32;
        else
            throw new CompilerException(this->line, this->file, "Unknown type " + this->name + " encountered!");
    }
}

uint16_t NType::getWordSize(AsmGenerator& context)
{
    return (int)std::ceil((double)this->getBitSize(context) / 16.0);
}

IType* NType::getExpressionType(AsmGenerator& context)
{
    // Types are of type type ;)
    return new NType(NType::TypeType);
}

bool NType::operator==(const NType& other) const
{
    return (this->name == other.name &&
            this->isStruct == other.isStruct &&
            this->isConstant == other.isConstant &&
            this->isSigned == other.isSigned &&
            this->pointerCount == other.pointerCount);
}

bool NType::operator!=(const NType& other) const
{
    return !(*this == other);
}
