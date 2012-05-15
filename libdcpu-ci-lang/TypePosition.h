/**

	File:		TypePosition.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang

	Authors:	James Rhodes

	Description:	Declares the TypePosition class.

**/

#ifndef __DCPU_LIBDCC_TYPEPOSITION_H
#define __DCPU_LIBDCC_TYPEPOSITION_H

#include <stdint.h>
#include <string>

class TypePosition
{
	private:
		bool m_Found;
		bool m_Global;
		bool m_Function;
		bool m_IsFunctionParameter;
		bool m_PreviousStackFrame;
		uint16_t m_Position;
		std::string m_FunctionName;
	public:
		TypePosition(bool isFound, bool isGlobal, bool isFunctionParameter, bool previousStackFrame, uint16_t position);
		TypePosition(bool isFound, std::string funcName);
		bool isFound();
		bool isFunction();
		std::string pushAddress(char registr);
};

#endif
