/**

    File:       NCaseStatement.cpp

    Project:    DCPU-16 Tools
    Component:  LibDCPU-ci-lang-c

    Authors:    Patrick Flick

    Description:    Defines the NCaseStatement AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NCaseStatement.h"
#include <sstream>

NCaseStatement::NCaseStatement(std::string value) :
    NStatement("case")
{
    std::stringstream str;
    str << (int)((uint8_t)((char)value[0]));
    this->m_constantLiteral = str.str();
    this->m_label = std::string("");
}

NCaseStatement::NCaseStatement(int value) :
    NStatement("case")
{
    std::stringstream str;
    str << (uint16_t)value;
    this->m_constantLiteral = str.str();
    this->m_label = std::string("");
}

void NCaseStatement::setLabelPrefix(std::string prefix)
{
    if (m_label == std::string(""))
    {
        std::stringstream str;
        str << prefix << m_constantLiteral;
        this->m_label = str.str();
    }
}

std::string NCaseStatement::getConstantLiteral()
{
    return this->m_constantLiteral;
}

AsmBlock* NCaseStatement::compile(AsmGenerator& context)
{
    AsmBlock* block = new AsmBlock();

    // Add file and line information.
    *block << this->getFileAndLineState();

    if (this->m_label == std::string(""))
        throw new CompilerException(this->line, this->file, "Invalid case statement outside of a switch statement.");

    // insert the label.
    *block << ":" << this->m_label << std::endl;

    return block;
}

AsmBlock* NCaseStatement::reference(AsmGenerator& context)
{
    throw new CompilerException(this->line, this->file, "Unable to get reference to the result of an return statement.");
}
