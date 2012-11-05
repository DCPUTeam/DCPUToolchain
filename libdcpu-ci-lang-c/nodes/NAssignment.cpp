/**

    File:           NAssignment.cpp

    Project:        DCPU-16 Tools
    Component:      LibDCPU-ci-lang-c

    Authors:        James Rhodes
                    Patrick Flick

    Description:    Defines the NAssignment AST class.

**/

// Prevents importing all of the class dependencies in
// the parser header so that we only get the token values.
#define YYSTYPE int

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "parser.hpp"
#include "NAssignment.h"
#include "NIdentifier.h"
#include <derr.defs.h>

AsmBlock* NAssignment::compile(AsmGenerator& context)
{
    AsmBlock* block = new AsmBlock();

    // Add file and line information.
    *block << this->getFileAndLineState();

    // When an assignment expression is referenced, the memory
    // address of the target goes into A.
    AsmBlock* las = this->lhs.reference(context);
    *block << *las;

    // push memory address
    *block <<   "   SET PUSH, A" << std::endl;
    delete las;

    // handle regular assignment as special case
    if (this->op == ASSIGN_EQUAL)
    {
        // When an expression is evaluated, the result goes into the A register.
        AsmBlock* rhs = this->rhs.compile(context);
        *block <<   *rhs;
        delete rhs;

        // cast to rhs to lhs type
        if (this->m_rhsType->implicitCastable(context, this->m_lhsType))
        {
            *block << *(this->m_rhsType->implicitCast(context, this->m_lhsType, 'A'));
        }

        // Pop the address of lhs into B
        if (context.isAssemblerDebug())
        {
            // Put the value into B and clear the
            // stack positions as we do so.
            *block <<   "   SET B, PEEK" << std::endl;
            *block <<   "   SET PEEK, 0" << std::endl;
            *block <<   "   ADD SP, 1" << std::endl;
        }
        else
        {
            // Not debugging, put the values into B.
            *block <<   "   SET B, POP" << std::endl;
        }

        // save the value A to [B]
        *block << *(this->m_lhsType->saveToRef(context, 'A', 'B'));
    }
    else
    {
        // When an expression is evaluated, the result goes into the A register.
        AsmBlock* rhs = this->rhs.compile(context);
        *block <<   *rhs;
        delete rhs;

        if (!this->m_IsPointerOp)
        {
            // cast to rhs to lhs type
            if (this->m_rhsType->implicitCastable(context, this->m_lhsType))
            {
                *block << *(this->m_rhsType->implicitCast(context, this->m_lhsType, 'A'));
            }
        }

        // move rhs over to register B
        *block <<   "   SET B, A" << std::endl;

        // get referenced value and put it in A
        *block <<   "   SET A, PEEK" << std::endl;
        *block << *(this->m_lhsType->loadFromRef(context, 'A', 'A'));

        // Now do the appropriate operation.
        // TODO a lot of assignment operations are missing !!
        // TODO type specific ops
        switch (this->op)
        {
            case ASSIGN_ADD:
                *block << *(this->m_lhsType->add(context, 'A', 'B'));
                break;

            case ASSIGN_SUBTRACT:
                *block << *(this->m_lhsType->sub(context, 'A', 'B'));
                break;

            case ASSIGN_MULTIPLY:
                *block << *(this->m_lhsType->mul(context, 'A', 'B'));
                break;

            case ASSIGN_DIVIDE:
                *block << *(this->m_lhsType->div(context, 'A', 'B'));
                break;

            case ASSIGN_MOD:
                *block << *(this->m_lhsType->mod(context, 'A', 'B'));
                break;

            case ASSIGN_BAND:
                *block << *(this->m_lhsType->band(context, 'A', 'B'));
                break;

            case ASSIGN_BOR:
                *block << *(this->m_lhsType->bor(context, 'A', 'B'));
                break;

            case ASSIGN_BXOR:
                *block << *(this->m_lhsType->bxor(context, 'A', 'B'));
                break;

            case ASSIGN_SHL:
                *block << *(this->m_lhsType->shl(context, 'A', 'B'));
                break;

            case ASSIGN_SHR:
                *block << *(this->m_lhsType->shr(context, 'A', 'B'));
                break;

            default:
                throw new CompilerException(this->line, this->file, "Unknown assignment operation requested.");
        }

        // Pop reference from stack
        // If debugging, clear the value as we POP them.
        if (context.isAssemblerDebug())
        {
            // Put the value into B and clear the
            // stack position as we do so.
            *block <<   "   SET B, PEEK" << std::endl;
            *block <<   "   SET PEEK, 0" << std::endl;
            *block <<   "   ADD SP, 1" << std::endl;
        }
        else
        {
            // Not debugging, put the values into B.
            *block <<   "   SET B, POP" << std::endl;
        }

        // Move the value into the target address.
        *block << *(this->m_lhsType->saveToRef(context, 'A', 'B'));
    }
    return block;
}

AsmBlock* NAssignment::reference(AsmGenerator& context)
{
    throw new CompilerException(this->line, this->file, "Unable to get reference to the result of an assignment.");
}

void NAssignment::analyse(AsmGenerator& context, bool reference)
{
    if (reference)
    {
        context.errorList.addError(this->line, this->file, ERR_CC_CANNOT_REFERENCE, " the result of assignment");
        return;
    }
    
    // first analyse the smaller parts
    this->lhs.analyse(context, true);
    this->rhs.analyse(context, false);
    
    // get lhs type
    this->m_lhsType = this->lhs.getExpressionType(context);
    
    // check whether this tries to write a const variable
    if(this->m_lhsType->isConst())
    {
        context.errorList.addError(this->line, this->file, ERR_CC_WRITE_TO_CONST, " the result of inline assembly");
    }
    
    // get rhs type
    this->m_rhsType = this->rhs.getExpressionType(context);
    
    // Check if both types are of a basic type
    this->m_IsPointerOp = false;
    if (this->op != ASSIGN_EQUAL)
    {
        if (this->m_lhsType->isPointer() && this->m_rhsType->isBasicType())
        {
            // pointer op: p += i, or p -= i
            this->m_IsPointerOp = true;
            if (this->op != ASSIGN_ADD && this->op != ASSIGN_SUBTRACT)
            {
                context.errorList.addError(this->line, this->file, ERR_CC_INVALID_ASSIGN_OPERANDS, "(have '"
                                            + this->m_lhsType->getName() + "' and '" + this->m_rhsType->getName() + "')");
            }
        }
        else if ((!this->m_rhsType->isBasicType()) || (!this->m_lhsType->isBasicType()))
        {
                context.errorList.addError(this->line, this->file, ERR_CC_INVALID_ASSIGN_OPERANDS, "(have '"
                                            + this->m_lhsType->getName() + "' and '" + this->m_rhsType->getName() + "')");
        }
    }
    
    if (!this->m_IsPointerOp)
    {
        // cast to rhs to lhs type
        if (!this->m_rhsType->implicitCastable(context, this->m_lhsType))
        {
            context.errorList.addError(this->line, this->file, ERR_CC_CANNOT_IMPLICIT_CAST, "'" + this->m_rhsType->getName()
                                        + "' to '" + this->m_lhsType->getName() + "'");
        }
    }
}

IType* NAssignment::getExpressionType(AsmGenerator& context)
{
    // An assignment has the type of it's LHS.
    return this->lhs.getExpressionType(context);
}
