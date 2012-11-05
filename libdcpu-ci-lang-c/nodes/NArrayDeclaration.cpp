/**

    File:       NArrayDeclaration.cpp

    Project:    DCPU-16 Tools
    Component:  LibDCPU-ci-lang-c

    Authors:    Patrick Flick

    Description:    Defines the NArrayDeclaration AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NArrayDeclaration.h"
#include "TPointer16.h"
#include "TArrayMemArea.h"
#include <SymbolTypes.h>
#include <derr.defs.h>

NArrayDeclaration::NArrayDeclaration(IType* type, NIdentifier& id, DimensionsList* dims, ExpressionList* initExprList) :
    id(id), m_dims(dims), m_baseType(type), m_initExprs(initExprList), NDeclaration("array")
{
    IType* t = type;
    uint16_t numPointers = 1;
    uint16_t numElements = 1;
    for (DimensionsList::iterator i = dims->begin(); i != dims->end(); ++i)
    {
        t = new TPointer16(t);
        t->setConst();
        numElements *= (*i);
        // if not last element
        if ((int32_t)(i - dims->begin()) + 1 < (int32_t)dims->size())
        {
            numPointers += numPointers * (*i);
        }
    }
    if (dims->size() < 2)
    {
        numPointers = 0;
    }

    this->m_pointerType = t;
    this->m_numElements = numElements;
    this->m_memAreaType = new TArrayMemArea(type, numElements, numPointers);
}

IType* NArrayDeclaration::getPointerType()
{
    return this->m_pointerType;
}

IType* NArrayDeclaration::getMemAreaType()
{
    return this->m_memAreaType;
}

AsmBlock* NArrayDeclaration::initPointerTable(AsmGenerator& context, char arrayPointerAdr, char arrayMemAreaAdr)
{
    // Create our new block.
    AsmBlock* block = new AsmBlock();

    // save the value A to [I]
    *block << *(this->m_pointerType->saveToRef(context, arrayMemAreaAdr, arrayPointerAdr));

    // if this is a multidimensional array, init the pointer table
    uint16_t acc_size = 1;
    for (unsigned int i = 0; i < this->m_dims->size() - 1; ++i)
    {
        // size of table row on current level
        acc_size *= (*this->m_dims)[i];
        // copy the memory location
        *block <<   "   SET " << arrayPointerAdr << ", " << arrayMemAreaAdr << std::endl;
        *block <<   "   ADD " << arrayPointerAdr << ", " << acc_size << std::endl;
        for (int j = 0; j < acc_size; ++j)
        {
            // set pointer in current mem slot
            // to [curPos+j] = curPos+ACC_SIZE[i]+j*LOCAL_SIZE[i+1]
            // where ACC_SIZE[i] = dims[0]*dims[1]*...*dims[i]
            // and LOCAL_SIZE[i+1] = dims[i+1]

            // TODO ops dependend on pointer type !?
            *block <<   "   SET [" << arrayMemAreaAdr << "], " << arrayPointerAdr << std::endl;
            *block <<   "   ADD " << arrayMemAreaAdr << ", 1" << std::endl;
            if (i + 1 != this->m_dims->size() - 1)
            {
                // dimension i+1 is still pointers
                *block <<   "   ADD " << arrayPointerAdr << ", " << (*this->m_dims)[i + 1] << std::endl;
            }
            else
            {
                // dimension i+1 is the last, use base type word size!
                *block <<   "   ADD " << arrayPointerAdr << ", " << this->m_baseType->getWordSize(context)*(*this->m_dims)[i + 1] << std::endl;
            }
        }
    }

    // init the array if it consists of structs (and it is not explicitly initialized)
    if (this->m_baseType->isStruct() && this->m_initExprs == NULL)
    {
        for (uint16_t i = 0; i < this->m_numElements; ++i)
        {
            *block << *(this->m_baseType->setToZero(context, arrayMemAreaAdr));
            *block <<   "   ADD " << arrayMemAreaAdr << ", " << this->m_baseType->getWordSize(context) << std::endl;
        }
    }

    return block;
}

AsmBlock* NArrayDeclaration::compile(AsmGenerator& context)
{
    // Create our new block.
    AsmBlock* block = new AsmBlock();

    // Add file and line information.
    *block << this->getFileAndLineState();

    // set up the pointer and the pointer table (for multi dims)

    // set the pointer that is the array variable
    *block << this->m_memPos.pushAddress('A');
    *block << this->m_variablePos.pushAddress('I');

    // init pointer table
    *block << *(this->initPointerTable(context, 'I', 'A'));

    if (this->m_initExprs == NULL)
        return block;

    // push data address onto stack
    *block <<   "   SET PUSH, A" << std::endl;

    for (uint16_t i = 0; i < this->m_numElements; ++i)
    {
        if (i < this->m_initExprs->size())
        {
            // When an expression is evaluated, the result goes into the A register.
            AsmBlock* expr = (*this->m_initExprs)[i]->compile(context);
            *block << *expr;
            delete expr;

            // cast to expr to array base type
            if (this->m_initExprTypes[i]->implicitCastable(context, this->m_baseType))
            {
                *block << *(this->m_initExprTypes[i]->implicitCast(context, this->m_baseType, 'A'));
            }

            // init in memory
            *block <<   "   SET B, POP" << std::endl;
            *block << *(this->m_baseType->saveToRef(context, 'A', 'B'));
            *block <<   "   ADD B, " << this->m_baseType->getWordSize(context) << std::endl;
            *block <<   "   SET PUSH, B" << std::endl;
        }
        else
        {
            // init with 0
            // TODO FIXME and do this more efficiently !
            //      e.g. two separate for loops, the second
            //      one is not using PUSH and POP
            *block <<   "   SET B, POP" << std::endl;
            *block << *(this->m_baseType->setToZero(context, 'B'));
            *block <<   "   ADD B, " << this->m_baseType->getWordSize(context) << std::endl;
            *block <<   "   SET PUSH, B" << std::endl;
        }
    }

    // pop value
    *block <<   "   ADD SP, 1" << std::endl;

    return block;
}

AsmBlock* NArrayDeclaration::reference(AsmGenerator& context)
{
    throw new CompilerException(this->line, this->file, "INTERNAL: Unable to get reference to an Array Declaration.");
}

void NArrayDeclaration::analyse(AsmGenerator& context, bool reference)
{

    // Get the position of the array variable.
    //TypePosition variablePos = context.m_CurrentFrame->getPositionOfVariable(this->id.name);
    this->m_variablePos = context.symbolTable->getPositionOfVariable(this->id.name);
    if (!this->m_variablePos.isFound())
    {
        context.errorList.addError(this->line, this->file, ERR_CC_VARIABLE_NOT_IN_SCOPE, this->id.name);
        return;
    }

    // Get the position of the mem area
    // TODO maybe move the "<arraymem>" internal tag into some unified variable
    this->m_memPos = context.symbolTable->getPositionOfVariable("<arraymem>_" + this->id.name);
    if (!this->m_memPos.isFound())
    {
        context.errorList.addError(this->line, this->file, ERR_CC_SPACE_FOR_ARRAY_NOT_ALLOC, this->id.name);
        return;
    }
    
    if (this->m_initExprs == NULL)
        return;
    
    // check that the init list is not bigger than the number of elements
    if (this->m_initExprs->size() > this->m_numElements)
    {
        context.errorList.addError(this->line, this->file, ERR_CC_EXCESS_ELEMENTS_IN_ARRAY_INIT, this->id.name);
        return;
    }
        
    this->m_initExprTypes = std::vector<IType*>(this->m_initExprs->size());
    for (uint16_t i = 0; i < this->m_initExprs->size(); ++i)
    {
        // FIXME:
        (*this->m_initExprs)[i]->analyse(context, false);

        // get type, it may has to be cast
        this->m_initExprTypes[i] = (*this->m_initExprs)[i]->getExpressionType(context);
        // cast to expr to array base type
        if (!this->m_initExprTypes[i]->implicitCastable(context, this->m_baseType))
        {
            context.errorList.addError(this->line, this->file, ERR_CC_CANNOT_IMPLICIT_CAST, "'" + this->m_initExprTypes[i]->getName()
                                        + "' to '" + this->m_baseType->getName() + "'");
        }
    }
}


void NArrayDeclaration::insertIntoScope(AsmGenerator& context, SymbolTableScope& scope, ObjectPosition position)
{
    scope.insert(this->id.name, VARIABLE_DECL, this->getPointerType(), this, position);
    // insert also the mem area for the data of the array
    scope.insert("<arraymem>_" + this->id.name, VARIABLE_DECL, this->getMemAreaType(), this, position);
}
