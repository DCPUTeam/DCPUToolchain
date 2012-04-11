/**

	File:			NFunctionDeclaration.cpp

	Project:		DCPU-16 Tools
	Component:		Compiler

	Authors:		James Rhodes

	Description:	Defines the NFunctionDeclaration AST class.

**/

#include "../asmgen.h"
#include "NFunctionDeclaration.h"

AsmBlock* NFunctionDeclaration::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();

	// Output a safety boundary if the assembler supports
	// it and we want to output in debug mode.
	if (context.isAssemblerDebug())
	{
		if (context.getAssembler().supportsSafetyBoundary)
			*block << ".BOUNDARY" << std::endl;
		else if (context.getAssembler().supportsDataInstruction)
			*block << "DAT 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0" << std::endl;
	}

	// Output the label.
	*block <<  ":cfunc_" << this->id.name << std::endl;

	// Calculate total stack space required.
	StackFrame* frame = context.generateStackFrame(this, false);

	// Now compile the block.
	AsmBlock* iblock = this->block.compile(context);
	*block << *iblock;
	delete iblock;

	// Return from this function.
	*block <<  "	SET A, 0xFFFF" << std::endl;
	*block <<  "	SET X, " << frame->getSize() << std::endl;
	*block <<  "	SET PC, _stack_return" << std::endl;

	// Clean up frame.
	context.finishStackFrame(frame);

	return block;
}

AsmBlock* NFunctionDeclaration::reference(AsmGenerator& context)
{
	throw new CompilerException("Unable to get reference to the result of a function declaration.");
}