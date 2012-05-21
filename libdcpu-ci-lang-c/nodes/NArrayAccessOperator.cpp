/**

	File:		NArrayAccessOperator.cpp

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	James Rhodes

	Description:	Defines the NArrayAccessOperator AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NType.h"
#include "NArrayAccessOperator.h"

AsmBlock* NArrayAccessOperator::compile(AsmGenerator& context)
{
	// We just use our own reference method and then
	// implicitly dereference here.
	AsmBlock* block = this->reference(context);
	*block << "	SET A, [A]" << std::endl;
	return block;
}

AsmBlock* NArrayAccessOperator::reference(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();

	// Add file and line information.
	*block << this->getFileAndLineState();

	// Get our type.
	NType* type = (NType*)this->getExpressionType(context);

	// Get a reference to the first component of the array access (result
	// will end up in A).
	AsmBlock* exprA = this->exprA.compile(context);
	*block << *exprA;
	*block << "	SET PUSH, A" << std::endl;
	delete exprA;

	// Evaluate the second component of the array access (result
	// will end up in B).
	AsmBlock* exprB = this->exprB.compile(context);
	*block << *exprB;
	*block << "	MUL A, " << type->getWordSize(context) << std::endl;
	*block << "	SET PUSH, A" << std::endl;
	delete type;
	delete exprB;

	// Add the two together.
	*block << "	SET B, POP" << std::endl;
	*block << "	SET A, POP" << std::endl;
	*block << "	ADD A, B" << std::endl;

	// The memory address is now in A, so we can simply return.
	return block;
}

IType* NArrayAccessOperator::getExpressionType(AsmGenerator& context)
{
	// An array operator has the type of it's "unpointered" first expression.
	NType* i = (NType*)this->exprA.getExpressionType(context);

	if (i->pointerCount > 0)
		i->pointerCount -= 1;
	else
	{
		// If we are using a literal or non-pointer as the first expression,
		// then we actually return void* as the type since it's array access
		// into unspecified memory.
		delete i;
		i = new NType(NType::VoidType);
		i->pointerCount += 1;
		return i;
	}

	return i;
}
