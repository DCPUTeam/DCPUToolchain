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

	// Handle function definitions.
	for (DeclarationList::iterator i = this->definitions.begin(); i != this->definitions.end(); i++)
	{
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