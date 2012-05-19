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

	// FIXME: get rid of the use of NType for function signatures!!
	if (funcsig == NULL)
	{
		// Try and get a variable with matching signature then.
		TypePosition varpos = context.m_CurrentFrame->getPositionOfVariable(this->id.name);

		if (!varpos.isFound())
			throw new CompilerException(this->line, this->file, "Neither a function nor a function pointer was found by the name '" + this->id.name + "'.");

		NType* vartype = (NType*)context.m_CurrentFrame->getTypeOfVariable(this->id.name);

		if (vartype->cType != "expression-identifier-type-function")
			throw new CompilerException(this->line, this->file, "Unable to call variable '" + this->id.name + "' as it is not a function pointer.");

		funcsig = (NFunctionSignature*)((NFunctionPointerType*)vartype);
		isDirect = false;
	}



	// check if the called function matches the signature size of this
	// method call
	if (this->arguments.size() != funcsig->arguments.size())
	{
		throw new CompilerException(this->line, this->file, 
		"Unable to find function\n"
		  "with singature:                               "
		+ this->id.name + this->calculateSignature(context)
		+ "\n"
		+ "Candidates are:                               "
		+ this->id.name + funcsig->getSignature());
	}	

	// Get the stack table of this method.
	StackFrame* frame = context.generateStackFrameIncomplete(funcsig);

	// Get a random label for our jump-back point.
	std::string jmpback = context.getRandomLabel("callback");

	// Evaluate each of the argument expressions in reverse
	// TODO make it depend on the typePosition somehow
	//      this here has to be exactly reverse to the order in the
	//      parameter stack frame and thus the TypePosition
	for (int i = this->arguments.size() - 1; i >= 0; --i)
	{
		// Compile the expression.
		AsmBlock* inst = this->arguments[i]->compile(context);
		*block << *inst;
		delete inst;
		
		IType* instType = this->arguments[i]->getExpressionType(context);

		// check types and cast implicitly
		IType* parameterType = funcsig->arguments[i]->type;
		if (instType->implicitCastable(parameterType))
		{
			// do cast
			*block << *(instType->implicitCast(parameterType, 'A'));
		}
		else
		{
			throw new CompilerException(this->line, this->file, 
			"Unable to find function\n"
			  "with singature:                               "
			+ this->id.name + this->calculateSignature(context)
			+ "\n"
			+ "Candidates are:                               "
			+ this->id.name + funcsig->getSignature());
		}

		// Push the result onto the stack.
		*block << *(parameterType->pushStack('A'));
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
	*block <<  "	JSR _stack_caller_init_overlap" << std::endl;

	// Then call the actual method and insert the return label.
	if (isDirect)
	{
		*block <<  "	SET PC, cfunc_" << this->id.name << std::endl;
	}
	else
	{
		// we are referencing the previous stack frame here
		// => parameter previousStackFrame=true
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

	// Clean up frame.
	context.finishStackFrame(frame);

	return block;
}

AsmBlock* NMethodCall::reference(AsmGenerator& context)
{
	throw new CompilerException(this->line, this->file, "Unable to get reference to the result of a method call.");
}

IType* NMethodCall::getExpressionType(AsmGenerator& context)
{
	// An method call has the type of the method's return type.
	NFunctionDeclaration* funcdecl = (NFunctionDeclaration*)context.getFunction(this->id.name);

	if (funcdecl == NULL)
		throw new CompilerException(this->line, this->file, "Called function was not found '" + this->id.name + "'.");

	IType* exprType = (IType*) funcdecl->type;
	return exprType;
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
		IType* type = (*i)->getExpressionType(context);
		sig = sig + type->getName();
	}
	sig = sig + ")";
	return sig;
}
