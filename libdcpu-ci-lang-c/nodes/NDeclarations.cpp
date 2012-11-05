/**

    File:       NDeclarations.cpp

    Project:    DCPU-16 Tools
    Component:  LibDCPU-ci-lang-c

    Authors:    James Rhodes

    Description:    Defines the NDeclarations AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NDeclarations.h"
#include "NFunctionDeclaration.h"
#include "NArrayDeclaration.h"
#include <derr.defs.h>

///
/// Defines the function that handles calling the standard
/// library entry point.
///


AsmBlock* NDeclarations::compile(AsmGenerator& context)
{
    AsmBlock* block = new AsmBlock();

    // Import bootstrap data.
    *block << ".IMPORT _stack_caller_init" << std::endl;
    *block << ".IMPORT _stack_caller_init_overlap" << std::endl;
    *block << ".IMPORT _stack_callee_return" << std::endl;
    *block << ".IMPORT cfunc__stdlib_enter" << std::endl;

    // Add file and line information.
    *block << this->getFileAndLineState();
    
    // Output the global data storage area.
    *block << std::endl;
    *block <<  ":_DATA" << std::endl;
    uint16_t globalDATs = this->m_globalDataSize;
    if (globalDATs > 0)
    {
        // Output zero'd data sections.
        *block <<  "; BEGIN: Global Data Space" << std::endl;
        
        // output in blocks of 10 DAT
        while (globalDATs >= 10)
        {
            *block <<  "    DAT 0";
            for (unsigned int b = 1; b < 10; b++)
            {
                *block <<  ", 0";
            }
            *block << std::endl;
            globalDATs -= 10;
        }
        
        if (globalDATs > 0)
        {
            // output the rest
            *block <<  "    DAT 0";
            for (unsigned int b = 1; b < globalDATs; b++)
            {
                *block <<  ", 0";
            }
            *block << std::endl;
        }
        *block <<  "; END: Global Data Space" << std::endl;
    }
    

    *block << std::endl;

    // If the assembler supports sections...
    if (context.getAssembler().supportsSections)
    {
        // Output the block for initializing global data storage.
        *block <<  ".SECTION INIT" << std::endl;

        for (DeclarationList::iterator i = this->definitions.begin(); i != this->definitions.end(); i++)
        {
            if ((*i)->cType == "statement-declaration-variable"
                    || (*i)->cType == "statement-declaration-array")
            {
                // Compile initializer.
                AsmBlock* inner = (*i)->compile(context);
                *block << *inner;

                if (inner != NULL)
                    delete inner;
            }
        }

        // Output the code section.
        *block <<  ".SECTION CODE" << std::endl;
    }

    // Deal with the main setup.
    if (this->m_hasMain)
    {
        // Output assembly for calling main.
        // NOTE: _stdlib_enter() should never return or bad things will happen!
        // NOTE: _stdlib_enter() always takes 0 parameters!
        *block <<  "    SET X, 0" << std::endl;
        *block <<  "    SET Z, 0" << std::endl;
        *block <<  "    JSR _stack_caller_init" << std::endl;
        *block <<  "    SET PC, cfunc__stdlib_enter" << std::endl;
    }

    // Handle function definitions.
    for (DeclarationList::iterator i = this->definitions.begin(); i != this->definitions.end(); i++)
    {
        if ((*i)->cType == "statement-declaration-variable"
                || (*i)->cType == "statement-declaration-array")
            continue;

        AsmBlock* inner = (*i)->compile(context);
        *block << *inner;

        if (inner != NULL)
            delete inner;
    }

    return block;
}

AsmBlock* NDeclarations::reference(AsmGenerator& context)
{
    // TODO: in this case: throw an internal compiler exception!
    throw new CompilerException(this->line, this->file, "INTERNAL: Unable to get reference to the result of a list-of-declarations node.");
}

void NDeclarations::analyse(AsmGenerator& context, bool reference)
{
    // Tell the generator that we are the root.
    context.m_RootNode = this;
    
    // insert declarations into scope
    // TODO do this inside the declarations themselves (during analyse())
    for (DeclarationList::iterator i = this->definitions.begin(); i != this->definitions.end(); i++)
    {
        (*i)->insertIntoScope(context, context.symbolTable->getGlobalScope(), GLOBAL);
    }
    
    // get size of global variables
    this->m_globalDataSize = context.symbolTable->getGlobalScope().getGlobalsSize();
    
    
    if (!context.getAssembler().supportsSections)
    {
        // Ensure we don't have any global variable initializers declared here.
        for (DeclarationList::iterator i = this->definitions.begin(); i != this->definitions.end(); i++)
        {
            if ((*i)->cType == "statement-declaration-variable")
                if (((NVariableDeclaration*)(*i))->initExpr != NULL)
                    context.errorList.addFatalError(this->line, this->file, ERR_CC_ASM_NO_SECTION_GLOBAL_INIT);
            if ((*i)->cType == "statement-declaration-array")
            {
                context.errorList.addFatalError(this->line, this->file, ERR_CC_ASM_NO_SECTION_GLOBAL_ARRAY);
            }
        }
    }
    
    // check if this has a implemented main function
    NFunctionDeclaration* main = (NFunctionDeclaration*)context.symbolTable->getFunction("main");
    // Check it's not just a declaration, but a definition.
    this->m_hasMain = (main != NULL && main->block != NULL);
    
    for (DeclarationList::iterator i = this->definitions.begin(); i != this->definitions.end(); i++)
    {
        (*i)->analyse(context, false);
    }
}
