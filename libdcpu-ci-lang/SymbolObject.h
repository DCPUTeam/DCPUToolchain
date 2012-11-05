/**

    File:           SymbolObject.h

    Project:        DCPU-16 Tools
    Component:      LibDCPU-ci-lang

    Authors:        Patrick Flick

    Description:    Declares the SymbolObject class.

**/

#ifndef __DCPU_LIBDCC_SYMBOLOBJECT_H
#define __DCPU_LIBDCC_SYMBOLOBJECT_H

class AsmGenerator;
class IDeclaration;

#include <string>
#include "nodes/IDeclaration.h"
#include "SymbolTypes.h"
#include "nodes/IType.h"


class SymbolObject
{
public:

public:
    std::string name;
    IDeclaration* decl;
    IType* type;
    DeclType declType;
    ObjectPosition objectPos;
    uint16_t positionOffset;
    //uint16_t wordSize;

public:
    SymbolObject() : name(""), decl(NULL), type(NULL),
        objectPos(LOCAL_STACK), positionOffset(0) {}
    SymbolObject(std::string name, DeclType declType, IType* type, IDeclaration* decl, ObjectPosition pos, uint16_t posOffset) :
        name(name), decl(decl), type(type), declType(declType), objectPos(pos), positionOffset(posOffset) {}
        
};

#endif
