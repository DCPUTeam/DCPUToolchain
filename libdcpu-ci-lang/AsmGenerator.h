/**

	File:		AsmGenerator.h

	Project:	DCPU-16 Toolchain
	Component:	LibDCPU-CI-Lang

	Authors:	James Rhodes

	Description:	Declares the AsmGenerator class.

**/

#ifndef __DCPU_LIBDCC_ASMGENERATOR_H
#define __DCPU_LIBDCC_ASMGENERATOR_H

#include <bstring.h>
#include <simclist.h>
#include <class.h>
#include <dcpu.h>
#include "Assembler.h"
#include "AsmBlock.h"
#include "StackFrame.h"
#include "nodes/IDeclarations.h"
#include "nodes/IFunctionDeclaration.h"
#include "nodes/IFunctionSignature.h"

// Class data.
#define MEMBERS_DATA_AsmGenerator				\
	const Assembler* m_AssemblerTarget;			\
	list_t m_AutomaticLabels;				\
	StackFrame* m_CurrentFrame;				\
	StackFrame* m_GlobalFrame;				\
	IDeclarations* m_RootNode;				\
 
// Class functions.
#define MEMBERS_FUNCTIONS_AsmGenerator											\
	char (*getRandomCharacter)(THIS(AsmGenerator));									\
	bstring (*getRandomString)(THIS(AsmGenerator), size_t size);							\
	IFunctionDeclaration* (*getFunction)(THIS(AsmGenerator), bstring name);						\
	StackFrame* (*generateStackFrame)(THIS(AsmGenerator), IFunctionDeclaration* function, bool referenceOnly);	\
	StackFrame* (*generateStackFrameIncomplete)(THIS(AsmGenerator), IFunctionSignature* signature);			\
	void (*finishStackFrame)(THIS(AsmGenerator), StackFrame* frame);						\
	bstring (*getRandomLabel)(THIS(AsmGenerator), bstring prefix);							\
	const Assembler* (*getAssembler)(THIS(AsmGenerator));								\
	bool (*isAssemblerDebug)(THIS(AsmGenerator));

// Class structure.
typedef struct
{
	CLASS_TYPE(AsmGenerator)
	CLASS_OPERATORS(AsmGenerator)
	MEMBERS_DATA_AsmGenerator
	MEMBERS_FUNCTIONS_AsmGenerator
} AsmGenerator;

// Class constructors and destructors.
AsmGenerator* new_AsmGenerator(bstring asmtarget);

#endif
