/**

	File:		NDeclarations.cpp

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	James Rhodes

	Description:	Defines the NDeclarations AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NDeclarations.h"
#include "NFunctionDeclaration.h"
#include "NArrayDeclaration.h"

AsmBlock* NDeclarations::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();

	// Import bootstrap data.
	*block << ".IMPORT _stack_caller_init" << std::endl;
	*block << ".IMPORT _stack_caller_init_overlap" << std::endl;
	*block << ".IMPORT _stack_callee_return" << std::endl;
	*block << ".IMPORT _halt" << std::endl;
	*block << ".IMPORT _halt_debug" << std::endl;

	// Add file and line information.
	*block << this->getFileAndLineState();

	// Tell the generator that we are the root.
	context.m_RootNode = this;

	// Create the global data frame.
	StackMap* map = new StackMap();

	for (DeclarationList::iterator i = this->definitions.begin(); i != this->definitions.end(); i++)
	{
		if ((*i)->cType == "statement-declaration-variable")
		{
			NVariableDeclaration* nvd = (NVariableDeclaration*)(*i);
			map->insert(map->end(), StackPair(nvd->id.name, nvd->type));
			// FIXME: Check to make sure variables do not clash with arguments
			//	  or other variable declarations (hint: check the map to
			//	  see if it already has a match).
		}
		else if ((*i)->cType == "statement-declaration-array")
		{
			// for arrays we have to push
			NArrayDeclaration* nad = (NArrayDeclaration*)(*i);
			std::string pointerName = nad->id.name;
			// TODO maybe move the "<arraymem>" internal tag into some unified variable
			std::string memAreaName = "<arraymem>_" + pointerName;
			IType* pointerType = nad->getPointerType();
			IType* memAreaType = nad->getMemAreaType();
			// insert the array pointer and the mem area into the map
			map->insert(map->end(), StackPair(pointerName, pointerType));
			map->insert(map->end(), StackPair(memAreaName, memAreaType));
			// FIXME: Check to make sure variables do not clash with arguments
			//	  or other variable declarations (hint: check the map to
			//	  see if it already has a match).
		}
	}

	context.m_GlobalFrame = new StackFrame(context, *map);

	// Output the global data storage area.
	*block << std::endl;
	*block <<  ":_DATA" << std::endl;

	for (DeclarationList::iterator i = this->definitions.begin(); i != this->definitions.end(); i++)
	{
		if ((*i)->cType == "statement-declaration-variable")
		{
			// Calculate size.
			unsigned int size = ((NVariableDeclaration*)(*i))->type->getWordSize(context);

			// We can't have types with 0 word storage in the global scope.
			if (size <= 0)
				throw new CompilerException(this->line, this->file, "Unable to store global variable with a type that has size of 0 words.");

			// Output zero'd data sections.
			*block <<  "	DAT 0";

			for (unsigned int b = 1; b < size; b++)
				*block <<  ", 0";

			*block << std::endl;
		}
		else if ((*i)->cType == "statement-declaration-array")
		{
			// Calculate size.
			unsigned int size = ((NArrayDeclaration*)(*i))->getPointerType()->getWordSize(context);
			size += ((NArrayDeclaration*)(*i))->getMemAreaType()->getWordSize(context);

			// We can't have types with 0 word storage in the global scope.
			if (size <= 0)
				throw new CompilerException(this->line, this->file, "Unable to store global variable with a type that has size of 0 words.");

			// Output zero'd data sections.
			*block <<  "	DAT 0";

			for (unsigned int b = 1; b < size; b++)
				*block <<  ", 0";

			*block << std::endl;
		}
	}

	*block << std::endl;

	// If the assembler supports sections...
	if (context.getAssembler().supportsSections)
	{
		// Output the block for initializing global data storage.
		*block <<  ".SECTION INIT" << std::endl;
		context.m_CurrentFrame = context.m_GlobalFrame; // So that the NVariableDeclaration compiles successfully.

		for (DeclarationList::iterator i = this->definitions.begin(); i != this->definitions.end(); i++)
		{
			if ((*i)->cType == "statement-declaration-variable"
				|| (*i)->cType == "statement-declaration-array")
			{
				// Compile initializer.
				AsmBlock* inner = (*i)->compile(context);
				*block << *inner;

				if (inner != NULL)
					delete inner;
			}
		}

		// Output the code section.
		*block <<  ".SECTION CODE" << std::endl;
	}
	else
	{
		// Ensure we don't have any global variable initializers declared here.
		for (DeclarationList::iterator i = this->definitions.begin(); i != this->definitions.end(); i++)
		{
			if ((*i)->cType == "statement-declaration-variable")
				if (((NVariableDeclaration*)(*i))->initExpr != NULL)
					throw new CompilerException(this->line, this->file, "Initializers not permitted on global variables for assemblers that don't support sections.");
			if ((*i)->cType == "statement-declaration-array")
			{
				throw new CompilerException(this->line, this->file, "Global arrays not permitted for assemblers that don't support sections.");
			}
		}
	}

	// Deal with the main setup.
	NFunctionDeclaration* main = (NFunctionDeclaration*)context.getFunction("main");
	if (main != NULL)
	{
		// Get the stack table of main.
		StackFrame* frame = context.generateStackFrame(main, false);

		// Output assembly for calling main.
		*block <<  "	SET X, " << frame->getParametersSize() << std::endl;
		*block <<  "	SET Z, _halt" << std::endl;
		*block <<  "	JSR _stack_caller_init" << std::endl;
		*block <<  "	SET PC, cfunc_main" << std::endl;

		// Clean up frame.
		context.finishStackFrame(frame);
	}

	// Handle function definitions.
	for (DeclarationList::iterator i = this->definitions.begin(); i != this->definitions.end(); i++)
	{
		if ((*i)->cType == "statement-declaration-variable"
			|| (*i)->cType == "statement-declaration-array")
			continue;

		AsmBlock* inner = (*i)->compile(context);
		*block << *inner;

		if (inner != NULL)
			delete inner;
	}

	return block;
}

AsmBlock* NDeclarations::reference(AsmGenerator& context)
{
	throw new CompilerException(this->line, this->file, "Unable to get reference to the result of a list-of-declarations node.");
}

IFunctionDeclaration* NDeclarations::getFunction(std::string name)
{
	// FIXME: IDeclarations must provide functionality to get a function by name.
	for (DeclarationList::iterator i = this->definitions.begin(); i != this->definitions.end(); i++)
	{
		if ((*i)->cType == "statement-declaration-function")
			if (((NFunctionDeclaration*)*i)->id.name == name)
				return (NFunctionDeclaration*)*i;
	}

	return NULL;
}
