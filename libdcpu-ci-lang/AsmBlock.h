/**

	File:		AsmBlock.h

	Project:	DCPU-16 Toolchain
	Component:	LibDCPU-CI-Lang

	Authors:	James Rhodes

	Description:	Declares the AsmBlock class.

**/

#ifndef __DCPU_LIBDCC_ASMBLOCK_H
#define __DCPU_LIBDCC_ASMBLOCK_H

#include <class.h>
#include <bstring.h>

// Class data.
#define MEMBERS_DATA_AsmBlock					\
	bstring m_Assembly;

// Class functions.
#define MEMBERS_FUNCTIONS_AsmBlock						\
	void (*write_char)(THIS(AsmBlock), char input);				\
	void (*write_const_size_t)(THIS(AsmBlock), const size_t input);		\
	void (*write_const_char)(THIS(AsmBlock), const char input[]);		\
	void (*write_stream)(THIS(AsmBlock), struct bStream* stream);		\
	void (*write_string)(THIS(AsmBlock), bstring input);			\
	void (*write_asmblock)(THIS(AsmBlock), struct _AsmBlock* block);

// Class structure.
typedef struct _AsmBlock
{
	CLASS_TYPE(AsmBlock)
	CLASS_OPERATORS(AsmBlock)
	MEMBERS_DATA_AsmBlock
	MEMBERS_FUNCTIONS_AsmBlock
} AsmBlock;

// Class constructors.
AsmBlock* new_AsmBlock();
void delete_AsmBlock(THIS(AsmBlock));

#endif
