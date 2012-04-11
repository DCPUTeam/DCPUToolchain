/**

	File:			NMethodCall.cpp

	Project:		DCPU-16 Tools
	Component:		Compiler

	Authors:		James Rhodes

	Description:	Defines the NMethodCall AST class.

**/

#include "../asmgen.h"
#include "NMethodCall.h"

AsmBlock* NMethodCall::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();
	
	// Get the function declaration.
	NFunctionDeclaration* funcdecl = context.getFunction(this->id.name);
	if (funcdecl == NULL)
		throw new CompilerException("Called function was not found '" + this->id.name + "'.");

	// Get the stack table of this method.
	StackFrame* frame = context.generateStackFrame(funcdecl);

	// Get a random label for our jump-back point.
	std::string jmpback = context.getRandomLabel("callback");
	
	// Copy a reference to the current position in
	// the stack first (by temporarily using register C, ugh!).
	*block <<  "	SET C, SP" << std::endl;

	// Evaluate each of the argument expressions.
	for (ExpressionList::iterator i = this->arguments.begin(); i != this->arguments.end(); i++)		
	{
		// Compile the expression.
		AsmBlock* inst = (*i)->compile(context);
		*block << *inst;
		delete inst;
		
		// Push the result onto the stack.
		*block <<  "	SET PUSH, A" << std::endl;
	}

	// Initialize the stack for this method.
	*block <<  "	SET X, " << frame->getSize() << std::endl;
	*block <<  "	SET Z, " << jmpback << std::endl;
	*block <<  "	JSR _stack_init" << std::endl;

	// Now copy each of the evaluated parameter values into
	// the correct parameter slots.
	uint16_t a = 1;
	for (VariableList::iterator v = funcdecl->arguments.begin(); v != funcdecl->arguments.end(); v++)		
	{
		// Get the location of the value.
		std::stringstream vstr;
		vstr << "[0x" << std::hex << (0x10000 - a) << "+C]";

		// Get the location of the slot.
		int32_t pos = frame->getPositionOfVariable((*v)->id.name);
		if (pos == -1)
			throw new CompilerException("The argument '" + (*v)->id.name + "' was not found in the argument list (internal error).");
		std::stringstream sstr;
		if (a == 0)
			sstr << "[Y]";
		else
			sstr << "[0x" << std::hex << pos << "+Y]"; // TODO: Optimize for 0 pos.

		// Now copy.
		*block <<	"	SET " << sstr.str() << ", " << vstr.str() << std::endl;

		// Increment.
		a += 1;
	}

	// Then call the actual method and insert the return label.
	*block <<  "	SET PC, cfunc_" << this->id.name << std::endl;
	*block <<  ":" << jmpback << std::endl;

	// Clean up all of our C values.
	for (int b = 0; b < a - 1; b += 1)
	{
		*block <<  "	SET PEEK, 0" << std::endl;
		*block <<  "	ADD SP, 1" << std::endl;
	}

	// Adjust Y frame by C amount.
	*block <<  "	ADD Y, " << (a - 1) << std::endl;

	// Clean up frame.
	context.finishStackFrame(frame);
	
	return block;
}

AsmBlock* NMethodCall::reference(AsmGenerator& context)
{
	throw new CompilerException("Unable to get reference to the result of a method call.");
}

NType& NMethodCall::getExpressionType(AsmGenerator& context)
{
	// An method call has the type of the method's return type.
	NFunctionDeclaration* funcdecl = context.getFunction(this->id.name);
	if (funcdecl == NULL)
		throw new CompilerException("Called function was not found '" + this->id.name + "'.");
	return (NType&)funcdecl->type;
}