/**

	File:		Assembler.h

	Project:	DCPU-16 Toolchain
	Component:	LibDCPU-CI-Lang

	Authors:	James Rhodes

	Description:	Declares the Assembler class.

**/

#ifndef __DCPU_LIBDCC_ASSEMBLER_H
#define __DCPU_LIBDCC_ASSEMBLER_H

#include <class.h>
#include <dcpu.h>

// Class data.
#define MEMBERS_DATA_Assembler							\
	/* Whether the assembler supports the .BOUNDARY directive. */		\
	bool supportsSafetyBoundary;						\
										\
	/* Whether the assembler supports .SECTION and .OUTPUT directives. */	\
	bool supportsSections;							\
										\
	/* Whether the assembler supports the DAT instruction. */		\
	bool supportsDataInstruction;						\
										\
	/* Whether the assembler supports next-value addresses of */		\
	/* the order [REG+ADDR] (in addition to [ADDR+REG]) */			\
	bool supportsReorderedAddresses;					\
										\
	/* Whether the assembler supports reading characters as */		\
	/* memory addresses using the 'a' notation. */				\
	bool supportsDirectCharacters;						\
										\
	/* Whether the assembler supports intermediate output and */		\
	/* the .IMPORT directive. */						\
	bool supportsLinkedImportDirective;					\
										\
	/* Whether the assembler supports intermediate output and */		\
	/* the .EXPORT directive. */						\
	bool supportsLinkedExportDirective;

// Class functions.
#define MEMBERS_FUNCTIONS_Assembler

// Class structure.
typedef struct
{
	CLASS_TYPE(Assembler)
	CLASS_OPERATORS(Assembler)
	MEMBERS_DATA_Assembler
	MEMBERS_FUNCTIONS_Assembler
} Assembler;

// Static functions.
void Assembler_loadAll();
bool Assembler_hasAssembler(std::string name);
const Assembler* Assembler_getAssembler(std::string name);

// Class constructors.
Assembler* new_Assembler(
	bool supportsSafetyBoundary,
	bool supportsSections,
	bool supportsDataInstruction,
	bool supportsReorderedAddresses,
	bool supportsDirectCharacters,
	bool supportsLinkedImportDirective,
	bool supportsLinkedExportDirective
);

#endif
