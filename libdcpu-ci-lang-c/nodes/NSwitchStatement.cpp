/**

    File:           NSwitchStatement.cpp

    Project:        DCPU-16 Tools
    Component:      LibDCPU-ci-lang-c

    Authors:        Patrick Flick

    Description:    Defines the NSwitchStatement AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NSwitchStatement.h"
#include "NCaseStatement.h"
#include "NDefaultStatement.h"
#include "Lists.h"
#include <derr.defs.h>

AsmBlock* NSwitchStatement::compile(AsmGenerator& context)
{
    AsmBlock* block = new AsmBlock();

    // Add file and line information.
    *block << this->getFileAndLineState();

    // When an expression is evaluated, the result goes into the A register.
    AsmBlock* eval = this->eval.compile(context);
    *block << *eval;
    delete eval;

    for (std::vector<std::string>::iterator it = this->m_cases.begin(); it != this->m_cases.end(); it++)
    {
        // If A equal to the case, set jump to the cases label
        *block <<   "   IFE A, " << *it << std::endl;
        *block <<   "       SET PC, " << this->m_caselbl << *it << std::endl;
    }

    if (this->m_defaultStmt == NULL)
    {
        *block <<   "   SET PC, " << this->m_endlbl << std::endl;
    }
    else
    {
        *block <<   "   SET PC, " << this->m_caselbl << "default" << std::endl;
    }

    // push stack for loop control statements
    context.pushLoopStack(this->m_endlbl, std::string(""));

    // Compile the main block.
    AsmBlock* expr = this->innerBlock.compile(context);
    *block << *expr;
    delete expr;

    // And insert the end label.
    *block << ":" << this->m_endlbl << std::endl;

    // pop stack for loop control statements
    context.popLoopStack();

    return block;
}

AsmBlock* NSwitchStatement::reference(AsmGenerator& context)
{
    throw new CompilerException(this->line, this->file, "Unable to get a reference to the result of a switch statement.");
}

void NSwitchStatement::analyse(AsmGenerator& context, bool reference)
{
    if (reference)
    {
        context.errorList.addError(this->line, this->file, ERR_CC_CANNOT_REFERENCE, " a switch statement");
        return;
    }
    
    // Create label for the while statement.
    this->m_endlbl = context.getRandomLabel("switch_end");
    this->m_caselbl = context.getRandomLabel("switch_case_");

    this->eval.analyse(context, false);

    // push stack for loop control statements
    context.pushLoopStack(this->m_endlbl, std::string(""));
    
    this->m_defaultStmt = NULL;
    this->m_cases = std::vector<std::string>();
    // loop through all statements looking for case or default statement
    // FIXME: let the case and default statements add themselves during analyse of block
    for (StatementList::iterator i = this->innerBlock.statements.begin(); i != this->innerBlock.statements.end(); i++)
    {
        if ((*i)->cType == "statement-case")
        {
            // output case
            NCaseStatement* ncs = (NCaseStatement*)(*i);

            // set label prefix
            ncs->setLabelPrefix(this->m_caselbl);
            
            this->m_cases.push_back(ncs->getConstantLiteral());
        }
        else if ((*i)->cType == "statement-default")
        {
            // default case
            NDefaultStatement* nds = (NDefaultStatement*)(*i);
            if (this->m_defaultStmt != NULL)
            {
                context.errorList.addError(this->line, this->file, ERR_CC_MULTIPLE_DEFAULT);
                return;
            }
            nds->setLabelPrefix(this->m_caselbl);
            this->m_defaultStmt = nds;
        }
    }
    
    this->innerBlock.analyse(context, false);
    
    // pop stack for loop control statements
    context.popLoopStack();
}
