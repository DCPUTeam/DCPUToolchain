/**

	File:		StackFrame.h

	Project:	DCPU-16 Toolchain
	Component:	LibDCPU-CI-Lang

	Authors:	James Rhodes

	Description:	Declares the StackFrame class.

**/

#ifndef __DCPU_LIBDCC_STACKFRAME_H
#define __DCPU_LIBDCC_STACKFRAME_H

class AsmGenerator;

#include <bstring.h>
#include <dcpu.h>
#include "AsmGenerator.h"
#include "StackMap.h"
#include "TypePosition.h"
#include "nodes/IType.h"

// Class data.
#define MEMBERS_DATA_StackFrame					\
	AsmGenerator* m_Generator;				\
	StackMap m_ParametersMap;				\
	StackMap m_LocalsMap;					\
	bool m_LocalsDefined;					\
 
// Class functions.
#define MEMBERS_FUNCTIONS_StackFrame										\
		TypePosition (*getPositionOfVariable)(THIS(StackFrame), bstring name, bool stackStartAtC);	\
		IType* (*getTypeOfVariable)(THIS(StackFrame), std::string name);				\
		uint16_t (*getParametersSize)(THIS(StackFrame));						\
		uint16_t (*getLocalsSize)(THIS(StackFrame));

// Class structure.
typedef struct
{
	CLASS_TYPE(StackFrame)
	CLASS_OPERATORS(StackFrame)
	MEMBERS_DATA_StackFrame
	MEMBERS_FUNCTIONS_StackFrame
} StackFrame;

// Class constructors.
StackFrame* new_StackFrame_GSS(AsmGenerator* generator, StackMap* parameters, StackMap* locals); // : m_ParametersMap(parameters), m_LocalsMap(locals), m_LocalsDefined(true), m_Generator(generator) { };
StackFrame* new_StackFrame_GS(AsmGenerator* generator, StackMap* parameters); // : m_ParametersMap(parameters), m_LocalsDefined(false), m_Generator(generator) { };

#endif
