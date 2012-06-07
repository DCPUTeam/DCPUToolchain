/**

	File:		NWhileStatement.cpp

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	James Rhodes

	Description:	Defines the NWhileStatement AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NWhileStatement.h"
#include "Lists.h"

AsmBlock* NWhileStatement::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();

	// Add file and line information.
	*block << this->getFileAndLineState();

	// Create label for the while statement.
	std::string startlbl = context.getRandomLabel("while");
	std::string endlbl = context.getRandomLabel("endwhile");

	// push stack for loop control statements
	context.pushLoopStack(endlbl, startlbl);

	// Output the start label.
	*block << ":" << startlbl << std::endl;

	// When an expression is evaluated, the result goes into the A register.
	AsmBlock* eval = this->eval.compile(context);
	*block << *eval;
	delete eval;

	// If A is not true, jump to the end.
	*block <<	"	IFN A, 0x1" << std::endl;
	*block <<	"		SET PC, " << endlbl << std::endl;

	// Compile the main block.
	AsmBlock* expr = this->expr.compile(context);
	*block << *expr;
	delete expr;

	// Jump back up to the start to do the evaluation.
	*block <<	"	SET PC, " << startlbl << std::endl;

	// And insert the end label.
	*block << ":" << endlbl << std::endl;
	
	// pop stack for loop control statements
	context.popLoopStack();

	return block;
}

AsmBlock* NWhileStatement::reference(AsmGenerator& context)
{
	throw new CompilerException(this->line, this->file, "Unable to get a reference to the result of a while statement.");
}
