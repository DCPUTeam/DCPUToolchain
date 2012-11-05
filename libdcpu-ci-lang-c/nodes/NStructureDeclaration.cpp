/**

    File:           NStructureDeclaration.cpp

    Project:        DCPU-16 Tools
    Component:      LibDCPU-ci-lang-c

    Authors:        James Rhodes, Patrick Flick

    Description:    Defines the NStructureDeclaration AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NStructureDeclaration.h"
#include "NArrayDeclaration.h"
#include <SymbolTypes.h>
#include <derr.defs.h>

AsmBlock* NStructureDeclaration::compile(AsmGenerator& context)
{
    // There is nothing to do for a structure declaration as
    // everything is handled when we first use the structure.
    return new AsmBlock();
}

AsmBlock* NStructureDeclaration::reference(AsmGenerator& context)
{
    throw new CompilerException(this->line, this->file, "Unable to get reference to the result of a structure declaration.");
}

void NStructureDeclaration::analyse(AsmGenerator& context, bool reference)
{
    if (reference)
    {
        context.errorList.addError(this->line, this->file, ERR_CC_CANNOT_REFERENCE, " a struct declaration");
        return;
    }
    
    // FIXME: implement this
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
