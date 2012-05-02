/**

	File:		NFunctionDeclaration.cpp

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	James Rhodes

	Description:	Defines the NFunctionDeclaration AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NFunctionDeclaration.h"
#include "NFunctionSignature.h"

NFunctionDeclaration::NFunctionDeclaration(const NType& type, const NIdentifier& id, const VariableList& arguments, NBlock* block)
	: id(id), block(block), pointerType(NULL), NDeclaration("function"), NFunctionSignature(type, arguments)
{
	// We need to generate an NFunctionPointerType for when we are resolved
	// as a pointer (for storing a reference to us into a variable).
	this->pointerType = new NFunctionPointerType(type, arguments);
}

NFunctionDeclaration::~NFunctionDeclaration()
{
	delete this->pointerType;
}

AsmBlock* NFunctionDeclaration::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();

	// If this function does not have a code block, then we assume
	// this function will be imported at some stage.
	if (this->block == NULL)
	{
		if (context.getAssembler().supportsLinkedImportDirective)
			*block <<  ".IMPORT cfunc_" << this->id.name << std::endl;
		else
			throw new CompilerException("Can't declare a function with no body without linker support in the target assembler.");

		return block;
	}

	// Output a safety boundary if the assembler supports
	// it and we want to output in debug mode.
	if (context.isAssemblerDebug())
	{
		if (context.getAssembler().supportsSafetyBoundary)
			*block << ".BOUNDARY" << std::endl;
		else if (context.getAssembler().supportsDataInstruction)
			*block << "DAT 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0" << std::endl;
	}

	// If the assembler supports exporting symbols, automatically
	// export this function.
	if (context.getAssembler().supportsLinkedExportDirective)
		*block <<  ".EXPORT cfunc_" << this->id.name << std::endl;

	// Calculate total stack space required.
	StackFrame* frame = context.generateStackFrame(this, false);

	// Output the leading information and immediate jump.
	*block <<  ":cfunc_" << this->id.name << std::endl;
	*block <<  "	SET PC, cfunc_" << this->id.name << "_actual" << std::endl;
	*block <<  "	DAT " << frame->getSize() << std::endl;
	*block <<  ":cfunc_" << this->id.name << "_actual" << std::endl;

	// Now compile the block.
	AsmBlock* iblock = this->block->compile(context);
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

StackMap NFunctionDeclaration::generateStackMap()
{
	StackMap map;

	// This function can not be called on functions with no body (because
	// that means they were an import and hence have no information
	// about the variables).
	if (this->block == NULL)
		throw new CompilerException("Can not generate a stack frame for a function declaration with no body.");

	// Add stack frame data for arguments.
	for (VariableList::const_iterator i = this->arguments.begin(); i != this->arguments.end(); i++)
	{
		map.insert(map.end(), StackPair((*i)->id.name, (*i)->type));
		// FIXME: Check to make sure arguments do not clash with any
		//	  other argument declarations (hint: check the map to
		//	  see if it already has a match).
	}

	// Add stack frame data for variable declarations.
	for (StatementList::iterator i = this->block->statements.begin(); i != this->block->statements.end(); i++)
	{
		if ((*i)->cType == "statement-declaration-variable")
		{
			NVariableDeclaration* nvd = (NVariableDeclaration*)(*i);
			map.insert(map.end(), StackPair(nvd->id.name, nvd->type));
			// FIXME: Check to make sure variables do not clash with arguments
			//	  or other variable declarations (hint: check the map to
			//	  see if it already has a match).
		}
	}

	return map;
}

IType* NFunctionDeclaration::getPointerType()
{
	return this->pointerType;
}
