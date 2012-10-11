/**

	File:		TArrayMemArea.cpp

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	Patrick Flick

	Description:	Defines the TArrayMemArea Type class.

**/
#define YYSTYPE int

#include <AsmGenerator.h>
#include <CompilerException.h>
#include "TArrayMemArea.h"

uint16_t TArrayMemArea::getWordSize(AsmGenerator& context)
{
	return (this->m_numPointers + (this->m_length * this->m_baseType->getWordSize(context)));
}

