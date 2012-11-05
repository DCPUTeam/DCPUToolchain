/**

    File:           NWhileStatement.cpp

    Project:        DCPU-16 Tools
    Component:      LibDCPU-ci-lang-c

    Authors:        James Rhodes, Patrick Flick

    Description:    Defines the NWhileStatement AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NWhileStatement.h"
#include "Lists.h"
#include <derr.defs.h>

AsmBlock* NWhileStatement::compile(AsmGenerator& context)
{
    AsmBlock* block = new AsmBlock();

    // Add file and line information.
    *block << this->getFileAndLineState();

    // push stack for loop control statements
    context.pushLoopStack(this->m_endlbl, this->m_startlbl);

    // Output the start label.
    *block << ":" << this->m_startlbl << std::endl;

    // When an expression is evaluated, the result goes into the A register.
    AsmBlock* eval = this->eval.compile(context);
    *block << *eval;
    delete eval;

    // If A is not true, jump to the end.
    *block <<   "   IFE A, 0x0" << std::endl;
    *block <<   "       SET PC, " << this->m_endlbl << std::endl;

    // Compile the main block.
    AsmBlock* expr = this->expr.compile(context);
    *block << *expr;
    delete expr;

    // Jump back up to the start to do the evaluation.
    *block <<   "   SET PC, " << this->m_startlbl << std::endl;

    // And insert the end label.
    *block << ":" << this->m_endlbl << std::endl;

    // pop stack for loop control statements
    context.popLoopStack();

    return block;
}

AsmBlock* NWhileStatement::reference(AsmGenerator& context)
{
    throw new CompilerException(this->line, this->file, "Unable to get a reference to the result of a while statement.");
}

void NWhileStatement::analyse(AsmGenerator& context, bool reference)
{
    if (reference)
    {
        context.errorList.addError(this->line, this->file, ERR_CC_CANNOT_REFERENCE, " a while statement");
        return;
    }
    
    // Create label for the while statement.
    this->m_startlbl = context.getRandomLabel("while");
    this->m_endlbl = context.getRandomLabel("endwhile");

    // push stack for loop control statements
    context.pushLoopStack(this->m_endlbl, this->m_startlbl);
    
    this->eval.analyse(context, false);
    this->expr.analyse(context, false);
    
    // pop stack for loop control statements
    context.popLoopStack();
}
