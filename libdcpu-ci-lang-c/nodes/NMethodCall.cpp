/**

	File:		NMethodCall.cpp

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	James Rhodes
					Patrick Flick
	Description:	Defines the NMethodCall AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NMethodCall.h"
#include "NFunctionDeclaration.h"
#include "NFunctionSignature.h"
#include "NFunctionPointerType.h"

AsmBlock* NMethodCall::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();

	// Add file and line information.
	*block << this->getFileAndLineState();

	// Get the function declaration.
	bool isDirect = true;
	NFunctionSignature* funcsig = (NFunctionDeclaration*)context.getFunction(this->id.name);

	if (funcsig == NULL)
	{
		// Try and get a variable with matching signature then.
		TypePosition varpos = context.m_CurrentFrame->getPositionOfVariable(this->id.name);

		if (!varpos.isFound())
			throw new CompilerException("Neither a function nor a function pointer was found by the name '" + this->id.name + "'.");

		NType* vartype = (NType*)context.m_CurrentFrame->getTypeOfVariable(this->id.name);

		if (vartype->cType != "expression-identifier-type-function")
			throw new CompilerException("Unable to call variable '" + this->id.name + "' as it is not a function pointer.");

		funcsig = (NFunctionSignature*)((NFunctionPointerType*)vartype);
		isDirect = false;
	}

	// check if the called function matches the signature of this method call
	// typedef std::vector<NVariableDeclaration*> VariableList; funcsig->arguments
	// typedef std::vector<NExpression*> ExpressionList for this->arguments
	// first check if argument length are the same
	if (this->arguments.size() != funcsig->arguments.size())
	{
		throw new CompilerException("There is no function with the name "
					    + this->id.name + " and signature " + this->calculateSignature(context) + "\n"
					    + "Candidates are:\t" + this->id.name + " with signature " + funcsig->getSignature());
	}
	// now check types of all the arguments
	for (unsigned int i = 0; i < funcsig->arguments.size(); i++)
	{
		NType callerType = (NType&) this->arguments[i]->getExpressionType(context);
		NType calleeType = (NType&) funcsig->arguments[i]->type;
		if (callerType.name != calleeType.name)
		{
			throw new CompilerException("There is no function with the name "
						    + this->id.name + " and signature " + this->calculateSignature(context) + "\n"
						    + "Candidates are:\t" + this->id.name + " with signature " + funcsig->getSignature());
		}
	}

	// Get the stack table of this method.
	StackFrame* frame = context.generateStackFrameIncomplete(funcsig);

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
	if (isDirect)
	{
		*block <<  "	SET X, cfunc_" << this->id.name << std::endl;
		*block <<  "	ADD X, 2" << std::endl;
	}
	else
	{
		TypePosition varpos = context.m_CurrentFrame->getPositionOfVariable(this->id.name);
		*block <<  varpos.pushAddress('X');
		*block <<  "	SET X, [X]" << std::endl;
		*block <<  "	ADD X, 2" << std::endl;
	}

	*block <<  "	SET X, [X]" << std::endl;
	*block <<  "	SET Z, " << jmpback << std::endl;
	*block <<  "	JSR _stack_init" << std::endl;

	// Now copy each of the evaluated parameter values into
	// the correct parameter slots.
	uint16_t a = 1;

	for (VariableList::const_iterator v = funcsig->arguments.begin(); v != funcsig->arguments.end(); v++)
	{
		// Get the location of the value.
		std::stringstream vstr;
		vstr << "[0x" << std::hex << (0x10000 - a) << "+C]";

		// Get the location of the slot.
		TypePosition result = frame->getPositionOfVariable((*v)->id.name);

		if (!result.isFound())
			throw new CompilerException("The argument '" + (*v)->id.name + "' was not found in the argument list (internal error).");

		// Now copy.
		*block << result.pushAddress('I');
		*block <<	"	SET [I], " << vstr.str() << std::endl;

		// Increment.
		a += 1;
	}

	// Then call the actual method and insert the return label.
	if (isDirect)
	{
		*block <<  "	SET PC, cfunc_" << this->id.name << std::endl;
	}
	else
	{
		TypePosition varpos = context.m_CurrentFrame->getPositionOfVariable(this->id.name, true);
		*block <<  varpos.pushAddress('X');
		*block <<  "	SET X, [X]" << std::endl;
		*block <<  "	SET PC, X" << std::endl;

		// TODO: In debug mode, there should be additional checks here to see if
		//	 the value that is going to be jumped to is 0 (NULL) so that it can
		//	 be reported back without doing weird stuff (like restarting the
		//	 program!)
	}

	*block <<  ":" << jmpback << std::endl;

	// Clean up all of our C values.
	for (int b = 0; b < a - 1; b += 1)
	{
		*block <<  "	SET PEEK, 0" << std::endl;
		*block <<  "	ADD SP, 1" << std::endl;
	}

	// TODO this has become unnessecary with the new bootstrap stack handleing
	// TODO	 maybe we can get rid of the C register alltogether?? (not sure)
	// Adjust Y frame by C amount
	//*block <<  "	ADD Y, " << (a - 1) << std::endl;

	// Clean up frame.
	context.finishStackFrame(frame);

	return block;
}

AsmBlock* NMethodCall::reference(AsmGenerator& context)
{
	throw new CompilerException("Unable to get reference to the result of a method call.");
}

IType& NMethodCall::getExpressionType(AsmGenerator& context)
{
	// An method call has the type of the method's return type.
	NFunctionDeclaration* funcdecl = (NFunctionDeclaration*)context.getFunction(this->id.name);

	if (funcdecl == NULL)
		throw new CompilerException("Called function was not found '" + this->id.name + "'.");

	return (NType&)funcdecl->type;
}


/*  This function gets a signature for compiler error output and does not */
/*  include the return type. Thus it is not compatible to */
/*  NFunctionSignature::calculateSignature() */
std::string NMethodCall::calculateSignature(AsmGenerator& context)
{



	std::string sig = "(";
	for (ExpressionList::const_iterator i = this->arguments.begin(); i != this->arguments.end(); i++)
	{
		if (i != this->arguments.begin())
		{
			sig = sig + ",";
		}
		NType type = (NType&)(*i)->getExpressionType(context);
		sig = sig + type.name;
	}
	sig = sig + ")";
	return sig;
}
