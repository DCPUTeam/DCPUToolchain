/**

	File:		NForStatement.cpp

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	James Rhodes

	Description:	Defines the NForStatement AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NForStatement.h"

AsmBlock* NForStatement::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();

	// Create label for the while statement.
	std::string startlbl = context.getRandomLabel("for");
	std::string endlbl = context.getRandomLabel("endfor");

	// Do the initalization statement.
	AsmBlock* initEval = this->initEval.compile(context);
	*block << *initEval;
	delete initEval;

	// Output the start label.
	*block << ":" << startlbl << std::endl;

	// When an expression is evaluated, the result goes into the A register.
	AsmBlock* checkEval = this->checkEval.compile(context);
	*block << *checkEval;
	delete checkEval;

	// If A is not true, jump to the end.
	*block <<	"	IFN A, 0x1" << std::endl;
	*block <<	"		SET PC, " << endlbl << std::endl;

	// Compile the main block.
	AsmBlock* expr = this->expr.compile(context);
	*block << *expr;
	delete expr;

	// Do the loop statement.
	AsmBlock* loopEval = this->loopEval.compile(context);
	*block << *loopEval;
	delete loopEval;

	// Jump back up to the start to do the evaluation.
	*block <<	"	SET PC, " << startlbl << std::endl;

	// And insert the end label.
	*block << ":" << endlbl << std::endl;

	return block;
}

AsmBlock* NForStatement::reference(AsmGenerator& context)
{
	throw new CompilerException("Unable to get reference to the result of a for statement.");
}
