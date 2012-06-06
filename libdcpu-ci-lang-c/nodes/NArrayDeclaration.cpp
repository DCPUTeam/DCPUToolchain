/**

	File:		NArrayDeclaration.cpp

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	Patrick Flick

	Description:	Defines the NArrayDeclaration AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NArrayDeclaration.h"
#include "TPointer16.h"
#include "TArrayMemArea.h"

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
		*block <<   "	SET " << arrayPointerAdr << ", " << arrayMemAreaAdr << std::endl;
		*block <<   "	ADD " << arrayPointerAdr << ", " << acc_size << std::endl;
		for (int j = 0; j < acc_size; ++j)
		{
			// set pointer in current mem slot
			// to [curPos+j] = curPos+ACC_SIZE[i]+j*LOCAL_SIZE[i+1]
			// where ACC_SIZE[i] = dims[0]*dims[1]*...*dims[i]
			// and LOCAL_SIZE[i+1] = dims[i+1]

			// TODO ops dependend on pointer type !?
			*block <<   "	SET [" << arrayMemAreaAdr << "], " << arrayPointerAdr << std::endl;
			*block <<   "	ADD " << arrayMemAreaAdr << ", 1" << std::endl;
			if (i + 1 != this->m_dims->size() - 1)
			{
				// dimension i+1 is still pointers
				*block <<   "	ADD " << arrayPointerAdr << ", " << (*this->m_dims)[i + 1] << std::endl;
			}
			else
			{
				// dimension i+1 is the last, use base type word size!
				*block <<   "	ADD " << arrayPointerAdr << ", " << this->m_baseType->getWordSize(context)*(*this->m_dims)[i + 1] << std::endl;
			}
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

	// Get the position of the array variable.
	TypePosition variablePos = context.m_CurrentFrame->getPositionOfVariable(this->id.name);
	if (!variablePos.isFound())
		throw new CompilerException(this->line, this->file, "The variable '" + this->id.name + "' was not found in the scope.");

	// Get the position of the mem area
	// TODO maybe move the "<arraymem>" internal tag into some unified variable
	TypePosition memPos = context.m_CurrentFrame->getPositionOfVariable("<arraymem>_" + this->id.name);
	if (!memPos.isFound())
		throw new CompilerException(this->line, this->file, "The space for the array '" + this->id.name + "' was not allocated.");

	// set up the pointer and the pointer table (for multi dims)

	// set the pointer that is the array variable
	*block << memPos.pushAddress('A');
	*block << variablePos.pushAddress('I');

	// init pointer table
	*block << *(this->initPointerTable(context, 'I','A'));

	// If we have no initialization expression, we don't need to init
	if (this->m_initExprs == NULL)
		return block;

	// push data address onto stack
	*block <<   "	SET PUSH, A" << std::endl;

	if (this->m_initExprs->size() > this->m_numElements)
		throw new CompilerException(this->line, this->file,
					    "Excess elements in array initializer of array '"
					    + this->id.name + "'");

	for (uint16_t i = 0; i < this->m_numElements; ++i)
	{
		if (i < this->m_initExprs->size())
		{
			// When an expression is evaluated, the result goes into the A register.
			AsmBlock* expr = (*this->m_initExprs)[i]->compile(context);
			*block << *expr;
			delete expr;

			// get type, it may has to be cast
			IType* exprType = (*this->m_initExprs)[i]->getExpressionType(context);
			// cast to expr to array base type
			if (exprType->implicitCastable(context, this->m_baseType))
			{
				*block << *(exprType->implicitCast(context, this->m_baseType, 'A'));
			}
			else
			{
				throw new CompilerException(this->line, this->file,
							    "Unable to implicitly cast '" + exprType->getName()
							    + "' to '" + this->m_baseType->getName() + "'");
			}

			// init in memory
			*block <<   "	SET B, POP" << std::endl;
			*block << *(this->m_baseType->saveToRef(context, 'A', 'B'));
			*block <<   "	ADD B, " << this->m_baseType->getWordSize(context) << std::endl;
			*block <<   "	SET PUSH, B" << std::endl;
		}
		else
		{
			// init with 0
			// TODO FIXME this has to be a type specific 0
			// TODO FIXME and do this more efficiently !
			//		e.g. two separate for loops, the second
			//		one is not using PUSH and POP
			*block <<   "	SET B, POP" << std::endl;
			for (int j = 0; j < this->m_baseType->getWordSize(context); ++j)
			{
				*block <<   "	SET [B], 0" << std::endl;
				*block <<   "	ADD B, 1" << std::endl;
			}
			*block <<   "	SET PUSH, B" << std::endl;
		}
	}

	// pop value
	*block <<   "	ADD SP, 1" << std::endl;

	return block;
}

AsmBlock* NArrayDeclaration::reference(AsmGenerator& context)
{
	throw new CompilerException(this->line, this->file, "Unable to get reference to the result of a variable.");
}
