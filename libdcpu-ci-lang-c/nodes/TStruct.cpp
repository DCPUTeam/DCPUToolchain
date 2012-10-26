/**

    File:       TStruct.cpp

    Project:    DCPU-16 Tools
    Component:  LibDCPU-ci-lang-c

    Authors:    Patrick Flick

    Description:    Defines the TStruct Type class.

**/


#include "TStruct.h"
#include "NDeclarations.h"
#include "NVariableDeclaration.h"
#include "NArrayDeclaration.h"
#include "Lists.h"
#include <cmath>


std::string TStruct::getName() const
{
    return m_name;
}

std::string TStruct::getInternalName() const
{
    return m_name;
}

bool TStruct::isConst() const
{
    return this->m_isConst;
}

void TStruct::setConst()
{
    this->m_isConst = true;
}


bool TStruct::isBasicType() const
{
    return false;
}

bool TStruct::isPointer() const
{
    return false;
}

bool TStruct::isStruct() const
{
    return true;
}

bool TStruct::implicitCastable(AsmGenerator& context, const IType* toType)
{
    std::string to = toType->getInternalName();
    if (to == m_name)
    {
        return true;
    }
    else
    {
        return false;
    }
}
bool TStruct::explicitCastable(AsmGenerator& context, const IType* toType)
{
    std::string to = toType->getInternalName();
    if (to == m_name)
    {
        return true;
    }
    else
    {
        return false;
    }
}

AsmBlock* TStruct::implicitCast(AsmGenerator& context, const IType* toType, char a)
{
    if (!this->implicitCastable(context, toType))
    {
        throw new CompilerException(0, "<internal>",
                                    "Unable to implicitly cast integer (internal error).");
    }
    // dont do anything
    AsmBlock* block = new AsmBlock();
    return block;
}

AsmBlock* TStruct::explicitCast(AsmGenerator& context, const IType* toType, char a)
{
    if (!this->explicitCastable(context, toType))
    {
        throw new CompilerException(0, "<internal>",
                                    "Unable to implicitly cast integer (internal error).");
    }
    // dont do anything
    AsmBlock* block = new AsmBlock();
    return block;
}



void TStruct::resolveStruct()
{
    if (this->m_resolvedStruct != NULL)
        return;

    if (this->m_context == NULL)
        throw new CompilerException(0, "<internal>",
                                    "Context for struct not initialized (internal error).");

    // Search AST for struct nodes.
    NDeclarations* decls = (NDeclarations*)m_context->m_RootNode;

    for (DeclarationList::iterator i = decls->definitions.begin(); i != decls->definitions.end(); i++)
        if ((*i)->cType == "statement-declaration-structure")
            if (((NStructureDeclaration*)*i)->id.name == this->m_name)
            {
                this->m_resolvedStruct = ((NStructureDeclaration*)*i);
                return;
            }

    throw new CompilerException(0, "<internal>", "Unknown struct type " + this->m_name + " encountered!");
    // TODO throw type exception, which is catched outside the type-system
    //   from where it is rethrown including line and file information
}

void TStruct::initContext(AsmGenerator& context)
{
    this->m_context = &context;
}

size_t TStruct::getBitSize()
{
    // TODO count bits
    return this->getWordSize() * 16;
}

uint16_t TStruct::getWordSize()
{
    this->resolveStruct();
    return this->m_resolvedStruct->getWordSize(*m_context);
}

uint16_t TStruct::getWordSize(AsmGenerator& context)
{
    this->initContext(context);
    return this->getWordSize();
}

AsmBlock* TStruct::initStruct(AsmGenerator& context, char structAdr)
{
    this->initContext(context);
    this->resolveStruct();

    AsmBlock* block = new AsmBlock();

    uint16_t pos = 0;
    for (DeclarationList::iterator i = this->m_resolvedStruct->fields.begin(); i != this->m_resolvedStruct->fields.end(); i++)
    {
        if ((*i)->cType == "statement-declaration-variable")
        {
            NVariableDeclaration* nvd = (NVariableDeclaration*)(*i);
            if (nvd->type->isStruct())
            {
                TStruct* type = (TStruct*)nvd->type;
                *block << " SET PUSH, " << structAdr << std::endl;
                *block << " ADD " << structAdr << ", " << pos << std::endl;
                *block << *(type->initStruct(context, structAdr));
                *block << " SET " << structAdr << ", POP" << std::endl;
            }
            pos += nvd->type->getWordSize(context);
        }
        else if ((*i)->cType == "statement-declaration-array")
        {
            NArrayDeclaration* nad = (NArrayDeclaration*)(*i);

            *block << " SET PUSH, " << structAdr << std::endl;
            *block << " ADD " << structAdr << ", " << pos << std::endl;
            char memAdr;
            if (structAdr != 'I')
                memAdr = 'I';
            else
                memAdr = 'B';
            *block << " SET PUSH, " << memAdr << std::endl;
            *block << " SET " << memAdr << ", " << structAdr << std::endl;
            *block << " ADD " << memAdr << ", 1" << std::endl;

            *block << *(nad->initPointerTable(context, structAdr, memAdr));

            *block << " SET " << memAdr << ", POP" << std::endl;
            *block << " SET " << structAdr << ", POP" << std::endl;

            pos += nad->getPointerType()->getWordSize(*m_context);
            pos += nad->getMemAreaType()->getWordSize(*m_context);
        }
    }

    return block;
}

uint16_t TStruct::getStructFieldPosition(std::string name)
{
    // Resolve struct if not already done.
    this->resolveStruct();

    // Count up the position.
    uint16_t pos = 0;

    for (DeclarationList::iterator i = this->m_resolvedStruct->fields.begin(); i != this->m_resolvedStruct->fields.end(); i++)
    {
        if ((*i)->cType == "statement-declaration-variable")
        {
            if (((NVariableDeclaration*)(*i))->id.name == name)
                return pos;
            else
                pos += ((NVariableDeclaration*)(*i))->type->getWordSize(*m_context);
        }
        else if ((*i)->cType == "statement-declaration-array")
        {
            if (((NArrayDeclaration*)(*i))->id.name == name)
                return pos;
            else
            {
                pos += ((NArrayDeclaration*)(*i))->getPointerType()->getWordSize(*m_context);
                pos += ((NArrayDeclaration*)(*i))->getMemAreaType()->getWordSize(*m_context);
            }
        }
    }

    // If the field wasn't found...
    throw new CompilerException(0, "<internal>", "Unable to lookup field " + name + " in structure " + this->m_resolvedStruct->id.name + "!");
    // TODO throw type exception, which is catched outside the type-system
    //   from where it is rethrown including line and file information
}

IType* TStruct::getStructFieldType(std::string name)
{
    // Resolve struct if not already done.
    this->resolveStruct();

    // Count up the position.
    size_t pos = 0;

    for (DeclarationList::iterator i = this->m_resolvedStruct->fields.begin(); i != this->m_resolvedStruct->fields.end(); i++)
    {
        if ((*i)->cType == "statement-declaration-variable")
        {
            if (((NVariableDeclaration*)(*i))->id.name == name)
                return ((NVariableDeclaration*)(*i))->type;
        }
        else if ((*i)->cType == "statement-declaration-array")
        {
            if (((NArrayDeclaration*)(*i))->id.name == name)
                return ((NArrayDeclaration*)(*i))->getPointerType();
        }
    }

    return NULL;

    // If the field wasn't found...
    // throw new CompilerException(0, "<internal>", "Unable to lookup field " + name + " in structure " + this->m_resolvedStruct->id.name + "!");
    // TODO throw type exception, which is catched outside the type-system
    //   from where it is rethrown including line and file information
}


/* copy */

// indirect copy given references (copies values)
AsmBlock* TStruct::copyByRef(AsmGenerator& context, char fromRef, char toRef)
{

    AsmBlock* block = new AsmBlock();
    if (fromRef != toRef)
    {
        for (unsigned int pos = 0; pos < this->getWordSize(); ++pos)
        {
            *block <<   "   SET [" << toRef << "], [" << fromRef << "]" << std::endl;
            *block <<   "   ADD " << toRef << ", 1" << std::endl;
            *block <<   "   ADD " << fromRef << ", 1" << std::endl;
        }
        *block <<   "   SUB " << toRef << ", " << this->getWordSize() << std::endl;
        *block <<   "   SUB " << fromRef << ", " << this->getWordSize() << std::endl;
    }

    // reset the pointer table of the source
    // TODO FIXME only copy content, and leave the pointer tables as they are
    *block <<   "   SET PUSH, " << fromRef << std::endl;
    *block <<   "   SET PUSH, " << toRef << std::endl;
    *block << *(this->initStruct(context, toRef));
    *block <<   "   SET " << toRef << ", POP" << std::endl;
    *block <<   "   SET " << fromRef << ", POP" << std::endl;

    return block;
}

/* because all structs are referenced and there are no direct values, */
/* all the following functions map to copyByRef */

// direct copy via registers
AsmBlock* TStruct::copyValue(AsmGenerator& context, char from, char to)
{
    return copyByRef(context, from, to);
}
// saves value in "from" register into the reference
AsmBlock* TStruct::saveToRef(AsmGenerator& context, char from, char toRef)
{
    return copyByRef(context, from, toRef);
}
// load from a reference into a value
AsmBlock* TStruct::loadFromRef(AsmGenerator& context, char fromRef, char to)
{
    AsmBlock* block = new AsmBlock();
    if (fromRef != to)
    {
        *block <<   "   SET " << to << ", " << fromRef << std::endl;
    }
    return block;
}


AsmBlock* TStruct::setToZero(AsmGenerator& context, char toRef)
{
    AsmBlock* block = new AsmBlock();
    *block << *(this->initStruct(context, toRef));
    return block;
}

/*************/
/* stack ops */
/*************/

AsmBlock* TStruct::pushStack(AsmGenerator& context, char a)
{
    // TODO fix pointer tables
    AsmBlock* block = new AsmBlock();
    *block <<   "   SUB SP, " << this->getWordSize() << std::endl;
    for (unsigned int pos = 0; pos < this->getWordSize(); ++pos)
    {
        *block <<   "   SET PEEK, [" << a << "]" << std::endl;
        *block <<   "   ADD SP, 1" << std::endl;
        *block <<   "   ADD " << a << ", 1" << std::endl;
    }
    *block <<   "   SUB SP, " << this->getWordSize() << std::endl;
    *block <<   "   SUB " << a << ", " << this->getWordSize() << std::endl;

    // reset the pointer table of the source
    // TODO FIXME only copy content, and leave the pointer tables as they are
    *block <<   "   SET PUSH, " << a << std::endl;
    *block <<   "   SET " << a << ", SP" << std::endl;
    *block <<   "   ADD " << a << ", 1" << std::endl;
    *block << *(this->initStruct(context, a));
    *block <<   "   SET " << a << ", POP" << std::endl;

    return block;
}


// FIXME do i need this? virtual AsmBlock* pushStackByRef(char a);


AsmBlock* TStruct::popStack(AsmGenerator& context)
{
    AsmBlock* block = new AsmBlock();
    *block <<   "   ADD SP, " << this->getWordSize() << std::endl;
    return block;
}

AsmBlock* TStruct::popStackClean(AsmGenerator& context)
{
    AsmBlock* block = new AsmBlock();
    for (unsigned int pos = 0; pos < this->getWordSize(); ++pos)
    {
        *block <<   "   SET PEEK, 0" << std::endl;
        *block <<   "   ADD SP, 1" << std::endl;
    }
    return block;
}




