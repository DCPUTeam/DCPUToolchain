/**

	File:		StackFrame.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang

	Authors:	James Rhodes

	Description:	Declares the StackFrame class.

**/

#ifndef __DCPU_LIBDCC_STACKFRAME_H
#define __DCPU_LIBDCC_STACKFRAME_H

class AsmGenerator;

#include <string>
#include "AsmGenerator.h"
#include "StackMap.h"
#include "TypePosition.h"
#include "nodes/IType.h"

class StackFrame
{
	private:
		AsmGenerator& m_Generator;
		StackMap m_StackMap;

	public:
		StackFrame(AsmGenerator& generator, StackMap& map) : m_StackMap(map), m_Generator(generator) { };
		TypePosition getPositionOfVariable(std::string name, bool stackStartAtC = false);
		IType* getTypeOfVariable(std::string name);
		uint16_t getSize();
};

#endif
