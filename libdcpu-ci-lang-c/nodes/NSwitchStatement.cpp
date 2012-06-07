/**

	File:		NSwitchStatement.cpp

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	Patrick Flick

	Description:	Defines the NSwitchStatement AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NSwitchStatement.h"
#include "NCaseStatement.h"
#include "NDefaultStatement.h"
#include "Lists.h"

AsmBlock* NSwitchStatement::compile(AsmGenerator& context)
{
	AsmBlock* block = new AsmBlock();

	// Add file and line information.
	*block << this->getFileAndLineState();

	// Create label for the while statement.
	std::string endlbl = context.getRandomLabel("switch_end");
	std::string caselbl = context.getRandomLabel("switch_case_");

	// push stack for loop control statements
	context.pushLoopStack(endlbl, std::string(""));

	// When an expression is evaluated, the result goes into the A register.
	AsmBlock* eval = this->eval.compile(context);
	*block << *eval;
	delete eval;

	NDefaultStatement* defaultStmt = NULL;

	// loop through all statements looking for case or default statement
	for (StatementList::iterator i = this->innerBlock.statements.begin(); i != this->innerBlock.statements.end(); i++)
	{
		if ((*i)->cType == "statement-case")
		{
			// output case
			NCaseStatement* ncs = (NCaseStatement*) (*i);
			
			// set label prefix
			ncs->setLabelPrefix(caselbl);
			
			// If A equal to the case, set jump to the cases label
			*block <<	"	IFE A, " << ncs->getConstantLiteral() << std::endl;
			*block <<	"		SET PC, " << caselbl << ncs->getConstantLiteral() << std::endl;
		}
		else if ((*i)->cType == "statement-default")
		{
			// default case 
			NDefaultStatement* nds = (NDefaultStatement*) (*i);
			if (defaultStmt != NULL)
				throw new CompilerException(this->line, this->file, "Invalid multiple default statements within switch statement.");
				
			nds->setLabelPrefix(caselbl);
			defaultStmt = nds;
		}
	}

	if (defaultStmt == NULL)
	{
		*block <<	"	SET PC, " << endlbl << std::endl;
	}
	else
	{
		*block <<	"	SET PC, " << caselbl << "default" << std::endl;
	}

	// Compile the main block.
	AsmBlock* expr = this->innerBlock.compile(context);
	*block << *expr;
	delete expr;

	// And insert the end label.
	*block << ":" << endlbl << std::endl;
	
	// pop stack for loop control statements
	context.popLoopStack();

	return block;
}

AsmBlock* NSwitchStatement::reference(AsmGenerator& context)
{
	throw new CompilerException(this->line, this->file, "Unable to get a reference to the result of a switch statement.");
}
