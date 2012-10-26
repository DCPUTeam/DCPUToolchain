/**

    File:       Node.h

    Project:    DCPU-16 Tools
    Component:  LibDCPU-ci-lang-c

    Authors:    James Rhodes

    Description:    Declares the Node AST class.

**/

#include "Node.h"
#include <AsmBlock.h>

AsmBlock Node::getFileAndLineState() const
{
    AsmBlock result;
    result << ".ULINE " << this->line << " \"" << this->file << "\"";
    return result;
}
