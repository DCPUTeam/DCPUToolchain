/**

	File:		TArrayMemArea.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang-c

	Authors:	Patrick Flick

	Description:	Declares the TArrayMemArea Type class.

**/

#ifndef __DCPU_COMP_TYPES_ARRAY_MEM_AREA_H
#define __DCPU_COMP_TYPES_ARRAY_MEM_AREA_H

#include <AsmGenerator.h>
#include <stdexcept>
#include <CompilerException.h>
#include <stdint.h>
#include <nodes/IType.h>

/*
 *  This class is used as a simple type with the given word size
 *  and is used for allocating enough space on the local stack or global memory
 *  for array data.
 */
class TArrayMemArea: public IType
{
	private:
		IType* m_baseType;
		uint16_t m_length;
		uint16_t m_numPointers;

	public:
		// this just returns the given word size
		uint16_t getWordSize(AsmGenerator& context);

		TArrayMemArea(IType* baseType, uint16_t length, uint16_t numPointers) :
			m_baseType(baseType), m_length(length), m_numPointers(numPointers) {}
};
#endif

