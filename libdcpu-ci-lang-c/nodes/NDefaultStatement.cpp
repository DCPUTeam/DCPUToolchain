/**

    File:       NDefaultStatement.cpp

    Project:    DCPU-16 Tools
    Component:  LibDCPU-ci-lang-c

    Authors:    Patrick Flick

    Description:    Defines the NDefaultStatement AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NDefaultStatement.h"
#include <sstream>
#include <derr.defs.h>

void NDefaultStatement::setLabelPrefix(std::string prefix)
{
    if (m_label == std::string(""))
    {
        std::stringstream str;
        str << prefix << "default";
        this->m_label = str.str();
    }
}

AsmBlock* NDefaultStatement::compile(AsmGenerator& context)
{
    AsmBlock* block = new AsmBlock();

    // Add file and line information.
    *block << this->getFileAndLineState();

    // insert the label.
    *block << ":" << this->m_label << std::endl;

    return block;
}

AsmBlock* NDefaultStatement::reference(AsmGenerator& context)
{
    throw new CompilerException(this->line, this->file, "Unable to get reference to the result of an return statement.");
}

void NDefaultStatement::analyse(AsmGenerator& context, bool reference)
{
    if (reference)
    {
        context.errorList.addError(this->line, this->file, ERR_CC_CANNOT_REFERENCE, " a default statement");
        return;
    }
    
    if (this->m_label == std::string(""))
    {
        context.errorList.addError(this->line, this->file, ERR_CC_DEFAULT_OUTSIDE_OF_SWITCH);
        return;
    }
}
