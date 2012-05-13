/**

	File:		IType.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang

	Authors:	James Rhodes

	Description:	Declares the IType AST interface.

**/

#ifndef __DCPU_COMP_INTERFACES_TYPE_H
#define __DCPU_COMP_INTERFACES_TYPE_H

#include <stdint.h>
#include "../CompilerException.h"
#include "../AsmGenerator.h"

class IType
{
	public:
		virtual uint16_t getWordSize(AsmGenerator& context)
		{
			throw new CompilerException("Unable to get word size of unspecified type (internal error).");
		}
};

#endif
