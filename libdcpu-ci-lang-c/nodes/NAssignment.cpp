/**

	File:		NAssignment.cpp

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	James Rhodes

	Description:	Defines the NAssignment AST class.

**/

// Prevents importing all of the class dependencies in
// the parser header so that we only get the token values.
#define YYSTYPE int

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "../parser.hpp"
#include "NAssignment.h"

AsmBlock* NAssignment::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();

	// Add file and line information.
	*block << this->getFileAndLineState();

	// When an assignment expression is referenced, the memory
	// address of the target goes into A.
	AsmBlock* las = this->lhs.reference(context);
	*block << *las;
	
	// get lhs type
	IType* lhsType = this->lhs.getExpressionType(context);
	
	// push memory address
	*block <<	"	SET PUSH, A" << std::endl;
	delete las;

	// handle regular assignment as special case
	if (this->op == ASSIGN_EQUAL)
	{
		// When an expression is evaluated, the result goes into the A register.
		AsmBlock* rhs = this->rhs.compile(context);
		*block <<   *rhs;
		delete rhs;
		
		// get rhs type
		IType* rhsType = this->rhs.getExpressionType(context);
		
		// cast to rhs to lhs type
		if (rhsType->implicitCastable(context, lhsType))
		{
			*block << *(rhsType->implicitCast(context, lhsType, 'A'));
		}
		else
		{
			throw new CompilerException(this->line, this->file, 
			"Unable to implicitly cast '" + rhsType->getName()
			+ "' to '" + lhsType->getName() + "'");
		}
		
		// Pop the address of lhs into B
		if (context.isAssemblerDebug())
		{
			// Put the value into B and clear the
			// stack positions as we do so.
			*block <<	"	SET B, PEEK" << std::endl;
			*block <<	"	SET PEEK, 0" << std::endl;
			*block <<	"	ADD SP, 1" << std::endl;
		}
		else
		{
			// Not debugging, put the values into B.
			*block <<	"	SET B, POP" << std::endl;
		}
		
		// save the value A to [B]
		*block << *(lhsType->saveToRef(context, 'A', 'B'));
	}
	else
	{
		// When an expression is evaluated, the result goes into the A register.
		AsmBlock* rhs = this->rhs.compile(context);
		*block <<   *rhs;
		delete rhs;
		
		// get rhs type
		IType* rhsType = this->rhs.getExpressionType(context);
		
		// Check if both types are of a basic type
		bool isPointerOp = false;
		if (lhsType->isPointer() && rhsType->isBasicType())
		{
			// pointer op: p += i, or p -= i
			isPointerOp = true;
			if (this->op != ASSIGN_ADD && this->op != ASSIGN_SUBTRACT)
			{
				throw new CompilerException(this->line, this->file, 
				"Invalid operands to assign operation. (have '"
				+ lhsType->getName() + "' and '" + rhsType->getName() + "')");
			}
		}
		else if ((!rhsType->isBasicType()) || (!lhsType->isBasicType()))
		{
			throw new CompilerException(this->line, this->file, 
			"Invalid operands to assign operation. (have '"
			+ lhsType->getName() + "' and '" + rhsType->getName() + "')");
		}
		
		if (!isPointerOp)
		{
			// cast to rhs to lhs type
			if (rhsType->implicitCastable(context, lhsType))
			{
				*block << *(rhsType->implicitCast(context, lhsType, 'A'));
			}
			else
			{
				throw new CompilerException(this->line, this->file, 
				"Unable to implicitly cast '" + rhsType->getName()
				+ "' to '" + lhsType->getName() + "'");
			}
		}
		
		// move rhs over to register B
		*block <<	"	SET B, A" << std::endl;
		
		// get referenced value and put it in A	
		*block <<	"	SET A, PEEK" << std::endl;
		*block << *(lhsType->loadFromRef(context, 'A', 'A'));
		
		// Now do the appropriate operation.
		// TODO a lot of assignment operations are missing !!
		// TODO type specific ops
		switch (this->op)
		{
			case ASSIGN_ADD:
				*block << *(lhsType->add(context, 'A','B'));
				break;

			case ASSIGN_SUBTRACT:
				*block << *(lhsType->sub(context, 'A','B'));
				break;

			case ASSIGN_MULTIPLY:
				*block << *(lhsType->mul(context, 'A','B'));
				break;

			case ASSIGN_DIVIDE:
				*block << *(lhsType->div(context, 'A','B'));
				break;
				
			case ASSIGN_MOD:
				*block << *(lhsType->mod(context, 'A','B'));
				break;
				
			case ASSIGN_BAND:
				*block << *(lhsType->band(context, 'A','B'));
				break;
				
			case ASSIGN_BOR:
				*block << *(lhsType->bor(context, 'A','B'));
				break;
				
			case ASSIGN_BXOR:
				*block << *(lhsType->bxor(context, 'A','B'));
				break;
				
			case ASSIGN_SHL:
				*block << *(lhsType->shl(context, 'A','B'));
				break;
				
			case ASSIGN_SHR:
				*block << *(lhsType->shr(context, 'A','B'));
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
			*block <<	"	SET B, PEEK" << std::endl;
			*block <<	"	SET PEEK, 0" << std::endl;
			*block <<	"	ADD SP, 1" << std::endl;
		}
		else
		{
			// Not debugging, put the values into B.
			*block <<	"	SET B, POP" << std::endl;
		}

		// Move the value into the target address.
		*block << *(lhsType->saveToRef(context, 'A', 'B'));
	}
	return block;
}

AsmBlock* NAssignment::reference(AsmGenerator& context)
{
	throw new CompilerException(this->line, this->file, "Unable to get reference to the result of an assignment.");
}

IType* NAssignment::getExpressionType(AsmGenerator& context)
{
	// An assignment has the type of it's LHS.
	return this->lhs.getExpressionType(context);
}
