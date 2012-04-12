/**

	File:			NDeclarations.cpp

	Project:		DCPU-16 Tools
	Component:		Compiler

	Authors:		James Rhodes

	Description:	Defines the NDeclarations AST class.

**/

#include "../asmgen.h"
#include "NDeclarations.h"

AsmBlock* NDeclarations::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();

	// Tell the generator that we are the root.
	context.m_RootNode = this;

	// First deal with the main setup.
	NFunctionDeclaration* main = context.getFunction("main");
	if (main == NULL)
		throw new CompilerException("Called function was not found 'main'.");

	// Get the stack table of main.
	StackFrame* frame = context.generateStackFrame(main, false);

	// Output assembly for calling main.
	*block <<  "	SET X, " << frame->getSize() << std::endl;
	*block <<  "	SET Z, _halt" << std::endl;
	*block <<  "	JSR _stack_init" << std::endl;
	*block <<  "	SET PC, cfunc_main" << std::endl;
	
	// Clean up frame.
	context.finishStackFrame(frame);

	// Create the global data frame.
	StackFrame::StackMap* map = new StackFrame::StackMap();
	for (DeclarationList::iterator i = this->definitions.begin(); i != this->definitions.end(); i++)
	{
		if ((*i)->cType != "statement-declaration-variable")
			continue;
		map->insert(map->end(), StackFrame::StackPair(((NVariableDeclaration*)(*i))->id.name, ((NVariableDeclaration*)(*i))->type));
	}
	context.m_GlobalFrame = new StackFrame(context, *map);

	// Output the global data storage area.
	*block << std::endl;
	*block <<  ":_DATA" << std::endl;
	for (DeclarationList::iterator i = this->definitions.begin(); i != this->definitions.end(); i++)
	{
		if ((*i)->cType != "statement-declaration-variable")
			continue;
		
		// Calculate size.
		unsigned int size = ((NVariableDeclaration*)(*i))->type.getWordSize(context);

		// We can't have types with 0 word storage in the global scope.
		if (size <= 0)
			throw new CompilerException("Unable to store global variable with a type that has size of 0 words.");

		// Output zero'd data sections.
		*block <<  "	DAT 0";
		for (unsigned int b = 1; b < size; b++)
			*block <<  ", 0";
		*block << std::endl;
	}
	*block << std::endl;

	// Handle function definitions.
	for (DeclarationList::iterator i = this->definitions.begin(); i != this->definitions.end(); i++)
	{
		if ((*i)->cType == "statement-declaration-variable")
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
	throw new CompilerException("Unable to get reference to the result of a list-of-declarations node.");
}