/**

    File:       NStructureDeclaration.cpp

    Project:    DCPU-16 Tools
    Component:  LibDCPU-ci-lang-c

    Authors:    James Rhodes

    Description:    Defines the NStructureDeclaration AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NStructureDeclaration.h"
#include "NArrayDeclaration.h"
#include <SymbolTypes.h>

AsmBlock* NStructureDeclaration::compile(AsmGenerator& context)
{
    // There is nothing to do for a structure declaration as
    // everything is handled when we first use the structure.
    return NULL;
}

AsmBlock* NStructureDeclaration::reference(AsmGenerator& context)
{
    throw new CompilerException(this->line, this->file, "Unable to get reference to the result of a structure declaration.");
}

size_t NStructureDeclaration::getWordSize(AsmGenerator& context)
{
    // Return the size of each of the fields.
    size_t s = 0;

    for (DeclarationList::iterator i = this->fields.begin(); i != this->fields.end(); i++)
    {
        if ((*i)->cType == "statement-declaration-variable")
        {
            s += ((NVariableDeclaration*)(*i))->type->getWordSize(context);
        }
        else if ((*i)->cType == "statement-declaration-array")
        {
            s += ((NArrayDeclaration*)(*i))->getPointerType()->getWordSize(context);
            s += ((NArrayDeclaration*)(*i))->getMemAreaType()->getWordSize(context);
        }
    }
    return s;
}

void NStructureDeclaration::insertIntoScope(AsmGenerator& context, SymbolTableScope& scope, ObjectPosition position)
{
    scope.insert(this->id.name, STRUCT_DECL, NULL, this, position);
}
