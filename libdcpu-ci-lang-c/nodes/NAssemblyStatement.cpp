/**

    File:           NAssemblyStatement.cpp

    Project:        DCPU-16 Tools
    Component:      LibDCPU-ci-lang-c

    Authors:        James Rhodes, Patrick Flick

    Description:    Defines the NAssemblyStatement AST class.

**/

#include <iostream>
#include <debug.h>
#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NAssemblyStatement.h"
#include <derr.defs.h>

AsmBlock* NAssemblyStatement::compile(AsmGenerator& context)
{
    AsmBlock* block = new AsmBlock();

    // Add file and line information.
    *block << this->getFileAndLineState();

    // add output
    *block << this->m_resultAsm << std::endl;

    return block;
}

AsmBlock* NAssemblyStatement::reference(AsmGenerator& context)
{
    throw new CompilerException(this->line, this->file, "INNER: Unable to get reference to the result of inline assembly.");
}

void NAssemblyStatement::analyse(AsmGenerator& context, bool reference)
{
    if (reference)
    {
        context.errorList.addError(this->line, this->file, ERR_CC_CANNOT_REFERENCE, " the result of inline assembly");
        return;
    }
    
    // find all <variablename> tags in the asm:
    size_t pos = 0;
    size_t closing = 0;
    std::string clone = this->asmcode;
    // using a for loop so that on "continue" the pos=closing will still be executed
    for (pos = 0; (pos = clone.find("<", pos)) != std::string::npos; pos = closing)
    {
        closing = clone.find(">", pos);
        if (closing == std::string::npos)
        {
            // there is no matching '>' to the last found '<'
            context.errorList.addError(this->line, this->file, ERR_CC_MALFORMED_ASM_BLOCK);
            return;
        }
        std::string varname = clone.substr(pos+1, closing-pos-1);
        
        // check that there actually is a variable name
        if (varname.size() == 0)
        {
            context.errorList.addError(this->line, this->file, ERR_CC_VARIABLE_NOT_IN_SCOPE, "");
            continue;
        }
        
        // check if the user tries to reference something
        bool isReference = false;
        if (varname[0] == '&')
        {
            isReference = true;
            varname = varname.substr(1);
        }
        
        // get position of variable from symbol table
        TypePosition typePos = context.symbolTable->getPositionOfVariable(varname);
        
        // check if there actually is a variable with the requested name
        if (!typePos.isFound())
        {
            context.errorList.addError(this->line, this->file, ERR_CC_VARIABLE_NOT_IN_SCOPE, varname);
            continue;
        }
        
        // if the coder tries to access the value of the function,
        //  throw an error (this doesn't make any sense)
        if((!isReference) && typePos.isFunction())
        {
            context.errorList.addError(this->line, this->file, ERR_CC_VALUE_OF_FUNCTION, varname);
            continue;
        }
        
        if (isReference && (!typePos.isFunction()))
        {
            context.errorList.addError(this->line, this->file, ERR_CC_VAR_ASM_REF);
            continue;
        }
        
        // check if we can get an atomic address of the variable
        // (this is not possible of the variable is in a previous
        // stack frame)
        if (!typePos.isAtomiclyAddressable())
        {
            context.errorList.addError(this->line, this->file, ERR_CC_VAR_NOT_ATOMIC_ADR, varname);
            continue;
        }
        
        // replace the <varname> tag in the asm block with the 
        // address of the variable and dereference with [..]
        // if the value and not the address was requested
        std::string replaceWith;
        if (isReference)
            replaceWith = typePos.getAtomicAddress();
        else
            replaceWith = std::string("[") + typePos.getAtomicAddress() + "]";
        clone.replace(pos, closing-pos+1, replaceWith);
    }
    
    // save output
    this->m_resultAsm = clone;
}
