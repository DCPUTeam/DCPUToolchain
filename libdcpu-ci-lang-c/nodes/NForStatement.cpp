/**

    File:           NForStatement.cpp

    Project:        DCPU-16 Tools
    Component:      LibDCPU-ci-lang-c

    Authors:        James Rhodes, Patrick Flick

    Description:    Defines the NForStatement AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NForStatement.h"
#include <derr.defs.h>

AsmBlock* NForStatement::compile(AsmGenerator& context)
{
    AsmBlock* block = new AsmBlock();

    // Add file and line information.
    *block << this->getFileAndLineState();

    // push stack for loop control statements
    context.pushLoopStack(this->m_endlbl, this->m_continuelbl);

    // Do the initalization statement.
    AsmBlock* initEval = this->initEval.compile(context);
    *block << *initEval;
    delete initEval;

    // Output the start label.
    *block << ":" << this->m_startlbl << std::endl;

    // When an expression is evaluated, the result goes into the A register.
    AsmBlock* checkEval = this->checkEval.compile(context);
    *block << *checkEval;
    delete checkEval;

    // If A is not true, jump to the end.
    *block <<   "   IFN A, 0x1" << std::endl;
    *block <<   "       SET PC, " << this->m_endlbl << std::endl;

    // Compile the main block.
    AsmBlock* expr = this->expr.compile(context);
    *block << *expr;
    delete expr;

    // Output the continue label.
    *block << ":" << this->m_continuelbl << std::endl;

    // Do the loop statement.
    AsmBlock* loopEval = this->loopEval.compile(context);
    *block << *loopEval;
    delete loopEval;

    // Jump back up to the start to do the evaluation.
    *block <<   "   SET PC, " << this->m_startlbl << std::endl;

    // And insert the end label.
    *block << ":" << this->m_endlbl << std::endl;

    // pop stack for loop control statements
    context.popLoopStack();

    return block;
}

AsmBlock* NForStatement::reference(AsmGenerator& context)
{
    throw new CompilerException(this->line, this->file, "Unable to get reference to the result of a for statement.");
}

void NForStatement::analyse(AsmGenerator& context, bool reference)
{
    if (reference)
    {
        context.errorList.addError(this->line, this->file, ERR_CC_CANNOT_REFERENCE, " a for statement");
        return;
    }
    
    // Create label for the for statement.
    this->m_startlbl = context.getRandomLabel("for");
    this->m_endlbl = context.getRandomLabel("endfor");
    this->m_continuelbl = context.getRandomLabel("continuefor");
    
    // push stack for loop control statements
    context.pushLoopStack(this->m_endlbl, this->m_continuelbl);

    initEval.analyse(context, false);
    checkEval.analyse(context, false);
    expr.analyse(context, false);
    loopEval.analyse(context, false);

    // pop stack for loop control statements
    context.popLoopStack();
}
