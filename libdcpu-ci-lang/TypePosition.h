/**

	File:		TypePosition.h

	Project:	DCPU-16 Toolchain
	Component:	LibDCPU-CI-Lang

	Authors:	James Rhodes

	Description:	Declares the TypePosition class.

**/

#ifndef __DCPU_LIBDCC_TYPEPOSITION_H
#define __DCPU_LIBDCC_TYPEPOSITION_H

#include <stdint.h>
#include <bstring.h>

// Class data.
#define MEMBERS_DATA_TypePosition				\
	bool m_Found;						\
	bool m_Global;						\
	bool m_Function;					\
	bool m_IsFunctionParameter;				\
	bool m_PreviousStackFrame;				\
	uint16_t m_Position;					\
	bstring m_FunctionName;

// Class functions.
#define MEMBERS_FUNCTIONS_TypePosition				\
	bool (*isFound)(THIS(TypePosition));			\
	bool (*isFunction)(THIS(TypePosition));			\
	bstring (*getAddress)(THIS(TypePosition));		\
	bstring (*pushAddress)(THIS(TypePosition), char registr);

// Class structure.
typedef struct
{
	CLASS_TYPE(TypePosition)
	CLASS_OPERATORS(TypePosition)
	MEMBERS_DATA_TypePosition
	MEMBERS_FUNCTIONS_TypePosition
} TypePosition;

// Class constructors.
TypePosition* new_TypePosition_BBBBU(bool isFound, bool isGlobal, bool isFunctionParameter, bool previousStackFrame, uint16_t position);
TypePosition* new_TypePosition_BS(bool isFound, bstring funcName);

#endif
