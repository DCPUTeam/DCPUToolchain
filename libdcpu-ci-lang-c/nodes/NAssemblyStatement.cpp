/**

    File:       NAssemblyStatement.cpp

    Project:    DCPU-16 Tools
    Component:  LibDCPU-ci-lang-c

    Authors:    James Rhodes

    Description:    Defines the NAssemblyStatement AST class.

**/

#include <iostream>
#include <debug.h>
#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NAssemblyStatement.h"

void NAssemblyStatement::findreplace(std::string& str, const std::string find, const std::string replace)
{
    size_t pos = 0;
    while ((pos = str.find(find, pos)) != std::string::npos)
    {
        str.replace(pos, find.length(), replace);
        pos += replace.length();
    }
}

void NAssemblyStatement::finderrorglob(std::string& str, const std::string find)
{
    size_t pos = 0;
    while ((pos = str.find(find, pos)) != std::string::npos)
        throw new CompilerException(this->line, this->file, std::string("References to global variable '") + find + "' are not permitted in inline assembly.  Copy to temporary first.");
}

void NAssemblyStatement::finderrorref(std::string& str, const std::string find)
{
    size_t pos = 0;
    while ((pos = str.find(find, pos)) != std::string::npos)
        throw new CompilerException(this->line, this->file, std::string("Retrieving address of variable '") + find + "' is not permitted in inline assembly.  Copy address to temporary first.");
}

AsmBlock* NAssemblyStatement::compile(AsmGenerator& context)
{
    AsmBlock* block = new AsmBlock();

    // Add file and line information.
    *block << this->getFileAndLineState();

    // Perform the inline reference replacements.
    std::string clone = this->asmcode;
    
    /*
    for (StackMap::iterator i = context.m_GlobalFrame->m_LocalsMap.begin(); i != context.m_GlobalFrame->m_LocalsMap.end(); i++)
        this->finderrorglob(clone, std::string("<&") + i->first + ">");
    
    for (StackMap::iterator i = context.m_GlobalFrame->m_ParametersMap.begin(); i != context.m_GlobalFrame->m_ParametersMap.end(); i++)
        this->finderrorglob(clone, std::string("<&") + i->first + ">");
    
    
    for (StackMap::iterator i = context.m_CurrentFrame->m_LocalsMap.begin(); i != context.m_CurrentFrame->m_LocalsMap.end(); i++)
        this->finderrorref(clone, std::string("<&") + i->first + ">");
        
    for (StackMap::iterator i = context.m_CurrentFrame->m_ParametersMap.begin(); i != context.m_CurrentFrame->m_ParametersMap.end(); i++)
        this->finderrorref(clone, std::string("<&") + i->first + ">");
        
    
    for (StackMap::iterator i = context.m_GlobalFrame->m_LocalsMap.begin(); i != context.m_GlobalFrame->m_LocalsMap.end(); i++)
        this->finderrorglob(clone, std::string("<") + i->first + ">");
        
    for (StackMap::iterator i = context.m_GlobalFrame->m_ParametersMap.begin(); i != context.m_GlobalFrame->m_ParametersMap.end(); i++)
        this->finderrorglob(clone, std::string("<") + i->first + ">");
        
        
    for (StackMap::iterator i = context.m_CurrentFrame->m_LocalsMap.begin(); i != context.m_CurrentFrame->m_LocalsMap.end(); i++)
        this->findreplace(clone, std::string("<") + i->first + ">", std::string("[") + context.m_CurrentFrame->getPositionOfVariable(i->first).getAddress() + "]");
        
    for (StackMap::iterator i = context.m_CurrentFrame->m_ParametersMap.begin(); i != context.m_CurrentFrame->m_ParametersMap.end(); i++)
        this->findreplace(clone, std::string("<") + i->first + ">", std::string("[") + context.m_CurrentFrame->getPositionOfVariable(i->first).getAddress() + "]");

    */
    
    // find all <variablename> tags in the asm:
    size_t pos = 0;
    while ((pos = clone.find("<", pos)) != std::string::npos)
    {
        size_t closing = clone.find(">", pos);
        if (closing == std::string::npos)
        {
            // 
            throw new CompilerException(this->line, this->file, "Malformed assembler in inline asm, check that every '<' is accompanied by another '>'.");
        }
        std::string varname = clone.substr(pos+1, closing-pos-1);
        this->findreplace(clone, std::string("<") + varname+ ">", std::string("[") + context.symbolTable->getPositionOfVariable(varname).getAddress() + "]");
        // FIXME: this is just to remind me to fix it later (to catch errors)
        std::cout << "FIXME: fix inline asm!" << std::endl;
        pos++;
    }
    
    // TODO FIXME find variable with that name
    
    // Directly insert the assembly code.
    *block << clone << std::endl;

    return block;
}

AsmBlock* NAssemblyStatement::reference(AsmGenerator& context)
{
    throw new CompilerException(this->line, this->file, "Unable to get reference to the result of inline assembly.");
}
