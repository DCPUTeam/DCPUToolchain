/**

    File:           NBuiltInVaStart.cpp

    Project:        DCPU-16 Tools
    Component:      LibDCPU-ci-lang-c

    Authors:        Patrick Flick

    Description:    Defines the NMethodCall AST class.

**/

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "NBuiltInVaStart.h"
#include "TInt16.h"

AsmBlock* NBuiltInVaStart::compile(AsmGenerator& context)
{
    AsmBlock* block = new AsmBlock();

    // Add file and line information.
    *block << this->getFileAndLineState();

    // get reference of the va_list variable
    *block << *(this->vaListID.reference(context));
    *block << "SET PUSH, A" << std::endl;
    
    // get reference of the paramN function argument
    *block << *(this->paramNID.reference(context));
    
    // set vaList variable to address in A and add the size of the context
    *block << "SET B, POP" << std::endl;
    *block << "SET [B], A" << std::endl;
    *block << "ADD [B], " << this->paramNID.getExpressionType(context)->getWordSize(context) << std::endl;

    return block;
}

AsmBlock* NBuiltInVaStart::reference(AsmGenerator& context)
{
    throw new CompilerException(this->line, this->file, "Unable to get reference __builtin_va_start.");
}

IType* NBuiltInVaStart::getExpressionType(AsmGenerator& context)
{
    // FIXME return void
    return new TInt16("void");
}
