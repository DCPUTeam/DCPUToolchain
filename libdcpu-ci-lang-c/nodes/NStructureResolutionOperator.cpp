/**

    File:       NStructureResolutionOperator.cpp

    Project:    DCPU-16 Tools
    Component:  LibDCPU-ci-lang-c

    Authors:    James Rhodes

    Description:    Defines the NStructureResolutionOperator AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NStructureResolutionOperator.h"

void NStructureResolutionOperator::initStructType(AsmGenerator& context)
{
    if (this->m_structType == NULL)
    {
        // So on the left-hand side we have an expression that needs to
        // evaluate to a structure, the only expressions that can do that
        // are method calls with a structure return type, other structure
        // resolution operators and lvalues suitable for assignment.  Check
        // to make sure our expression is one of those before continuing.
        if (this->lhs.cType != "expression-methodcall" &&
                this->lhs.cType != "expression-dereference" &&
                this->lhs.cType != "expression-identifier" &&
                this->lhs.cType != "expression-field" &&
                this->lhs.cType != "expression-arrayaccess")
            throw new CompilerException(this->line, this->file, "Unable to use AST node " + this->lhs.cType + " as part of the structure resolution operator; it is not a suitable left-value.");

        // Ensure the LHS expression is actually a structure type.
        IType* lhsType = this->lhs.getExpressionType(context);

        if (!lhsType->isStruct())
            throw new CompilerException(this->line, this->file, "Unable to use AST node " + this->lhs.cType + " as part of the structure resolution operator; the resulting type is not a structure.");

        this->m_structType = this->m_structType = (TStruct*) lhsType;
        this->m_structType->initContext(context);
    }
}


AsmBlock* NStructureResolutionOperator::compile(AsmGenerator& context)
{
    AsmBlock* block = new AsmBlock();

    // Add file and line information.
    *block << this->getFileAndLineState();

    // Use our reference function to generate the location.
    AsmBlock* expr = this->reference(context);

    IType* fieldType = this->m_structType->getStructFieldType(this->rhs.name);

    // return the value of the field
    *block <<   *expr;
    *block <<   *(fieldType->loadFromRef(context, 'A', 'A'));
    delete expr;

    return block;
}

AsmBlock* NStructureResolutionOperator::reference(AsmGenerator& context)
{
    AsmBlock* block = new AsmBlock();

    // Add file and line information.
    *block << this->getFileAndLineState();

    // init
    this->initStructType(context);

    // We need to work out at what offset is the RHS identifier
    // in the structure.
    uint16_t pos = this->m_structType->getStructFieldPosition(this->rhs.name);

    // Evaluate the LHS, placing a reference to it's position on the stack.
    AsmBlock* lhs = this->lhs.reference(context);
    *block << *lhs;
    delete lhs;

    // Now recalculate the position of A by adding the position
    // of the variable onto it.
    *block <<   "   ADD A, " << pos << std::endl;

    return block;
}

void NStructureResolutionOperator::analyse(AsmGenerator& context, bool reference)
{
    // FIXME TODO implement this!!
    lhs.analyse(context, true);
    
}

IType* NStructureResolutionOperator::getExpressionType(AsmGenerator& context)
{
    // init variables, and check types
    this->initStructType(context);

    // get field type
    IType* fieldType = this->m_structType->getStructFieldType(this->rhs.name);

    if (fieldType == NULL)
        // Field not found
        throw new CompilerException(this->line, this->file,
                                    "Struct field '" + this->m_structType->getName() + "."
                                    + this->rhs.name + "' not found.");
    return fieldType;
}
