/**

    File:       NFunctionDeclaration.cpp

    Project:    DCPU-16 Tools
    Component:  LibDCPU-ci-lang-c

    Authors:    James Rhodes

    Description:    Defines the NFunctionDeclaration AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NFunctionDeclaration.h"
#include "NFunctionSignature.h"
#include "NArrayDeclaration.h"
#include <SymbolTypes.h>
#include <derr.defs.h>

NFunctionDeclaration::NFunctionDeclaration(const IType* type, const NIdentifier& id, const VariableList& arguments, NBlock* block, bool varArgs)
    : m_isDeclaration(false), id(id), block(block), pointerType(NULL), NDeclaration("function"), NFunctionSignature(type, arguments, varArgs)
{
    // We need to generate an NFunctionPointerType for when we are resolved
    // as a pointer (for storing a reference to us into a variable).
    this->pointerType = new NFunctionPointerType(type, arguments);
}

NFunctionDeclaration::~NFunctionDeclaration()
{
    delete this->pointerType;
}

AsmBlock* NFunctionDeclaration::compile(AsmGenerator& context)
{
    AsmBlock* block = new AsmBlock();

    // Add file and line information.
    *block << this->id.getFileAndLineState();

    if (this->m_isDeclaration)
    {
        *block <<  ".IMPORT cfunc_" << this->id.name << std::endl;
        return block;
    }

    // Output a safety boundary if the assembler supports
    // it and we want to output in debug mode.
    if (context.isAssemblerDebug())
    {
        if (context.getAssembler().supportsSafetyBoundary)
            *block << ".BOUNDARY" << std::endl;
        else if (context.getAssembler().supportsDataInstruction)
            *block << "DAT 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0" << std::endl;
    }

    // If the assembler supports exporting symbols, automatically
    // export this function.
    if (context.getAssembler().supportsLinkedExportDirective)
        *block <<  ".EXPORT cfunc_" << this->id.name << std::endl;
    

    context.symbolTable->beginScope(this->functionScope);
    context.initLoopStack();

    // Output the leading information and immediate jump.
    *block <<  ":cfunc_" << this->id.name << std::endl;
    *block <<  "    SET PC, cfunc_" << this->id.name << "_actual" << std::endl;
    *block <<  "    DAT " << this->functionScope->getParameterStackSize() << std::endl;
    *block <<  ":cfunc_" << this->id.name << "_actual" << std::endl;

    // Allocate locals.
    *block <<  "    SUB SP, " << this->functionScope->getLocalStackSize() << std::endl;

    // Now compile the block.
    AsmBlock* iblock = this->block->compile(context);
    *block << *iblock;
    delete iblock;

    // Free locals.
    *block <<  "    ADD SP, " << this->functionScope->getLocalStackSize() << std::endl;

    // Return from this function.
    *block <<  "    SET A, 0xFFFF" << std::endl;
    *block <<  "    SET X, " << this->functionScope->getParameterStackSize() << std::endl;
    *block <<  "    SET PC, _stack_callee_return" << std::endl;

    // Clean up frame.
    context.symbolTable->endScope();
    context.initLoopStack();

    return block;
}

AsmBlock* NFunctionDeclaration::reference(AsmGenerator& context)
{
    throw new CompilerException(this->line, this->file, "INTERNAL: Unable to get reference to the result of a function declaration.");
}


void NFunctionDeclaration::analyse(AsmGenerator& context, bool reference)
{
    // If this function does not have a code block, then we assume
    // this function will be imported at some stage.
    if (this->block == NULL)
    {
        if (context.getAssembler().supportsLinkedImportDirective)
            this->m_isDeclaration = true;
        else
            context.errorList.addFatalError(this->line, this->file, ERR_CC_ASM_NO_IMPORT_DECL_FUNC);
        return;
    }
    else
        this->m_isDeclaration = false;
    
    
    // set up new scope and fill it
    this->functionScope = context.symbolTable->beginNewScope();
    context.initLoopStack();
    
    for (VariableList::const_iterator i = this->arguments.begin(); i != this->arguments.end(); i++)
    {
        // add function parameters
        (*i)->insertIntoScope(context, context.symbolTable->getCurrentScope(), PARAMETER_STACK);
    }
    for (StatementList::iterator i = this->block->statements.begin(); i != this->block->statements.end(); i++)
    {
        // TODO do this cleanly, do this from inside the block ...
        if ((*i)->cType == "statement-declaration-variable" || (*i)->cType == "statement-declaration-array")
        {
            // add local variables
            ((NDeclaration*) *i)->insertIntoScope(context, context.symbolTable->getCurrentScope(), LOCAL_STACK);
        }
    }
    
    // call analyse function of all stmts
    this->block->analyse(context, false);
    
    context.symbolTable->endScope();
    context.initLoopStack();
}

IType* NFunctionDeclaration::getPointerType()
{
    return this->pointerType;
}

void NFunctionDeclaration::insertIntoScope(AsmGenerator& context, SymbolTableScope& scope, ObjectPosition position)
{
    scope.insert(this->id.name, FUNCTION_DECL, this->getPointerType(), (IFunctionDeclaration*) this, position);
}
