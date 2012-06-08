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
#include "NArrayDeclaration.h"

NFunctionDeclaration::NFunctionDeclaration(const IType* type, const NIdentifier& id, const VariableList& arguments, NBlock* block)
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

	// Add file and line information.
	*block << this->id.getFileAndLineState();

	// If this function does not have a code block, then we assume
	// this function will be imported at some stage.
	if (this->block == NULL)
	{
		if (context.getAssembler().supportsLinkedImportDirective)
			*block <<  ".IMPORT cfunc_" << this->id.name << std::endl;
		else
			throw new CompilerException(this->line, this->file, "Can't declare a function with no body without linker support in the target assembler.");

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
	context.initLoopStack();

	// Output the leading information and immediate jump.
	*block <<  ":cfunc_" << this->id.name << std::endl;
	*block <<  "	SET PC, cfunc_" << this->id.name << "_actual" << std::endl;
	*block <<  "	DAT " << frame->getParametersSize() << std::endl;
	*block <<  ":cfunc_" << this->id.name << "_actual" << std::endl;

	// Allocate locals.
	*block <<  "	SUB SP, " << frame->getLocalsSize() << std::endl;

	// Now compile the block.
	AsmBlock* iblock = this->block->compile(context);
	*block << *iblock;
	delete iblock;

	// Free locals.
	*block <<  "	ADD SP, " << frame->getLocalsSize() << std::endl;

	// Return from this function.
	*block <<  "	SET A, 0xFFFF" << std::endl;
	*block <<  "	SET X, " << frame->getParametersSize() << std::endl;
	*block <<  "	SET PC, _stack_callee_return" << std::endl;

	// Clean up frame.
	context.finishStackFrame(frame);
	context.initLoopStack();

	return block;
}

AsmBlock* NFunctionDeclaration::reference(AsmGenerator& context)
{
	throw new CompilerException(this->line, this->file, "Unable to get reference to the result of a function declaration.");
}

StackMap NFunctionDeclaration::generateLocalsStackMap()
{
	StackMap map;

	// This function can not be called on functions with no body (because
	// that means they were an import and hence have no information
	// about the variables).
	if (this->block == NULL)
		throw new CompilerException(this->line, this->file, "Can not generate a locals stack frame for a function declaration with no body.");

	// Add stack frame data for variable declarations.
	for (StatementList::iterator i = this->block->statements.begin(); i != this->block->statements.end(); i++)
	{
		if ((*i)->cType == "statement-declaration-variable")
		{
			NVariableDeclaration* nvd = (NVariableDeclaration*)(*i);
			
			// pseudo code for typedef resolval
			// 1. if nvd->type is a typedef:
			// 1.a resolve typedef type
			// 1.1 if type->baseDecl is an array declaration?
			// 1.1.1 replace variable declaration with new array declaration (direct replacement)
			// 1.1.2 something like --i, continue (replaced declaration, which is handled again)
			// 1.2 if type->baseDecl is a variable declaration?
			// 1.2.1 replace variable decl with the new typedef decl
			// 1.2.2 something like --i, continue
			
			map.insert(map.end(), StackPair(nvd->id.name, nvd->type));
			// FIXME: Check to make sure variables do not clash with arguments
			//	  or other variable declarations (hint: check the map to
			//	  see if it already has a match).
		}
		else if ((*i)->cType == "statement-declaration-array")
		{
			// pseudo code for typedef resolval
			// 1. if nad->basetype is a typedef:
			// 1.a resolve typedef type
			// 1.1 if type->baseDecl is an array declaration?
			// 1.1.1 merge array declarations:
			// 1.1.2 add dimensions of this decl to the array decl, creating a new array decl
			// 1.1.3 something like --i, continue (replaced declaration, which is handled again)
			// 1.2 if type->baseDecl is a variable declaration?
			// 1.2.1 replace with new array declaration of this type and dimensions from this
			// 	 currently handled array delaration
			// 1.2.2 --i, continue
			
			// for arrays we have to push both the pointer and
			// the memory area onto the stack
			NArrayDeclaration* nad = (NArrayDeclaration*)(*i);
			std::string pointerName = nad->id.name;
			std::string memAreaName = "<arraymem>_" + pointerName;
			IType* pointerType = nad->getPointerType();
			IType* memAreaType = nad->getMemAreaType();
			// insert the array pointer and the mem area into the map
			map.insert(map.end(), StackPair(pointerName, pointerType));
			map.insert(map.end(), StackPair(memAreaName, memAreaType));
			// FIXME: Check to make sure variables do not clash with arguments
			//	  or other variable declarations (hint: check the map to
			//	  see if it already has a match).
		}
	}

	return map;
}

StackMap NFunctionDeclaration::generateParametersStackMap()
{
	StackMap map;

	// Add stack frame data for arguments.
	for (VariableList::const_iterator i = this->arguments.begin(); i != this->arguments.end(); i++)
	{
		// TODO same thing here, replace typedefs
		
		map.insert(map.end(), StackPair((*i)->id.name, (*i)->type));
		// FIXME: Check to make sure arguments do not clash with any
		//	  other argument declarations (hint: check the map to
		//	  see if it already has a match).
	}

	return map;
}

IType* NFunctionDeclaration::getPointerType()
{
	return this->pointerType;
}
