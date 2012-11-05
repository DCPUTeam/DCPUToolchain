/**

    File:           NVariableDeclaration.cpp

    Project:        DCPU-16 Tools
    Component:      LibDCPU-ci-lang-c

    Authors:        James Rhodes, Patrick Flick

    Description:    Defines the NVariableDeclaration AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NVariableDeclaration.h"
#include "TStruct.h"
#include <SymbolTypes.h>
#include <derr.defs.h>

AsmBlock* NVariableDeclaration::compile(AsmGenerator& context)
{
    // Create our new block.
    AsmBlock* block = new AsmBlock();

    // get variable type
    IType* varType = this->type;

    // if this is a struct, it may has to be initialized (arrays within structs)
    if (varType->isStruct())
    {
        // Set the value of the variable directly.
        *block << this->m_varPos.pushAddress('I');
        TStruct* structType = (TStruct*) varType;
        *block << *(structType->initStruct(context, 'I'));
    }

    // If we have no initialization expression, we don't need to do anything.
    if (this->initExpr == NULL)
        return block;

    // Add file and line information.
    *block << this->getFileAndLineState();

    // When an expression is evaluated, the result goes into the A register.
    AsmBlock* expr = this->initExpr->compile(context);
    *block << *expr;
    delete expr;


    // cast to rhs to lhs type
    if (this->m_exprType->implicitCastable(context, varType))
    {
        *block << *(this->m_exprType->implicitCast(context, varType, 'A'));
    }

    // Set the value of the variable directly.
    *block << this->m_varPos.pushAddress('I');

    // save the value A to [I]
    *block << *(varType->saveToRef(context, 'A', 'I'));

    return block;
}

AsmBlock* NVariableDeclaration::reference(AsmGenerator& context)
{
    throw new CompilerException(this->line, this->file, "Unable to get reference to the result of a variable.");
}

void NVariableDeclaration::analyse(AsmGenerator& context, bool reference)
{
    if (reference)
    {
        context.errorList.addError(this->line, this->file, ERR_CC_CANNOT_REFERENCE, " a variable declaration");
        return;
    }
    
    // Get the position of the variable.
    this->m_varPos = context.symbolTable->getPositionOfVariable(this->id.name);

    if (!this->m_varPos.isFound())
    {
        context.errorList.addError(this->line, this->file, ERR_CC_VARIABLE_NOT_IN_SCOPE, this->id.name);
        return;
    }
    
    if (this->initExpr == NULL)
        return;
    
    this->initExpr->analyse(context, false);
    
    // get type, it may has to be cast
    this->m_exprType = this->initExpr->getExpressionType(context);

    // cast to rhs to lhs type
    if (!this->m_exprType->implicitCastable(context, this->type))
    {
            context.errorList.addError(this->line, this->file, ERR_CC_CANNOT_IMPLICIT_CAST, "'" + this->m_exprType->getName()
                                        + "' to '" + this->type->getName()  + "'");
    }
}

void NVariableDeclaration::insertIntoScope(AsmGenerator& context, SymbolTableScope& scope, ObjectPosition position)
{
    if (scope.contains(this->id.name))
    {
        throw new CompilerException(this->line, this->file, "Error: Variable with name '" + this->id.name + "' was declared before.");
    }
    else if (scope.containsRec(this->id.name))
    {
        // TODO maybe warn??
    }
    scope.insert(this->id.name, VARIABLE_DECL, this->type, this, position);
}
