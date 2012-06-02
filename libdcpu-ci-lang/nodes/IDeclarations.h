/**

	File:		IDeclarations.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang

	Authors:	James Rhodes

	Description:	Declares the IDeclarations AST interface.

**/

#ifndef __DCPU_COMP_INTERFACES_DECLARATIONS_H
#define __DCPU_COMP_INTERFACES_DECLARATIONS_H

#include <bstring.h>
#include "IFunctionDeclaration.h"

// Class data.
#define MEMBERS_DATA_IDeclarations

// Class functions.
#define MEMBERS_FUNCTIONS_IDeclarations				\
	IFunctionDeclaration* (*getFunction)(bstring name);

// Class structure.
typedef struct
{
	CLASS_TYPE(IDeclarations)
	CLASS_OPERATORS(IDeclarations)
	MEMBERS_DATA_IDeclarations
	MEMBERS_FUNCTIONS_IDeclarations
} IDeclarations;

#endif
