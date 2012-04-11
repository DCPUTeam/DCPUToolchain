/**

	File:			NIfStatement.cpp

	Project:		DCPU-16 Tools
	Component:		Compiler

	Authors:		James Rhodes

	Description:	Defines the NIfStatement AST class.

**/

#include "../asmgen.h"
#include "NIfStatement.h"

AsmBlock* NIfStatement::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();
	
	// Create labels for the if statement.
	std::string truelbl = context.getRandomLabel("if");
	std::string falselbl = "";
	std::string endlbl = context.getRandomLabel("end");
	if (this->if_false != NULL)
		falselbl = context.getRandomLabel("else");

	// When an expression is evaluated, the result goes into the A register.
	AsmBlock* expr = this->eval.compile(context);
	*block << *expr;
	delete expr;

	// Check the value of A to see where the flow should go.
	*block <<		"	IFE A, 0x1" << std::endl;
	*block <<		"		SET PC, " << truelbl << std::endl;
	if (this->if_false != NULL)
		*block <<	"	SET PC, " << falselbl << std::endl;
	else
		*block <<	"	SET PC, " << endlbl << std::endl;

	// Compile the true block.
	AsmBlock* trueblk = this->if_true.compile(context);
	*block << ":" << truelbl << std::endl;
	*block << *trueblk;
	*block <<	"	SET PC, " << endlbl << std::endl;
	delete trueblk;

	// Compile the false block if we have one.
	if (this->if_false != NULL)
	{
		AsmBlock* falseblk = this->if_false->compile(context);
		*block << ":" << falselbl << std::endl;
		*block << *falseblk;
		*block <<	"	SET PC, " << endlbl << std::endl;
		delete falseblk;
	}

	// And insert the end label.
	*block << ":" << endlbl << std::endl;

	return block;
}

AsmBlock* NIfStatement::reference(AsmGenerator& context)
{
	throw new CompilerException("Unable to get reference to the result of an if statement.");
}