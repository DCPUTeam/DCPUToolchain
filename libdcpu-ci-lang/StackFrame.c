/**

	File:		StackFrame.cpp

	Project:	DCPU-16 Toolchain
	Component:	LibDCPU-CI-Lang

	Authors:	James Rhodes

	Description:	Defines the StackFrame class.

**/

/*
#include "StackFrame.h"

// Gets a relative stack position by the variable name.
TypePosition StackFrame::getPositionOfVariable(std::string name, bool previousStackFrame)
{
	uint16_t size = 0;

	// Search parameters.
	for (StackMap::iterator i = this->m_ParametersMap.begin(); i != this->m_ParametersMap.end(); i++)
	{
		if ((*i).first == name)
			return TypePosition(true, this->m_Generator.m_GlobalFrame == this, true, previousStackFrame, size);
		else
			size += (*i).second->getWordSize(this->m_Generator);
	}

	// reset count for local stack
	size = 0;

	// Search locals.
	for (StackMap::iterator i = this->m_LocalsMap.begin(); i != this->m_LocalsMap.end(); i++)
	{
		if ((*i).first == name)
			return TypePosition(true, this->m_Generator.m_GlobalFrame == this, false, previousStackFrame, this->getLocalsSize() - size);
		else
			size += (*i).second->getWordSize(this->m_Generator);
	}

	// Search globals.
	if (this->m_Generator.m_GlobalFrame == NULL || this->m_Generator.m_GlobalFrame == this)
	{
		// Also search defined functions.
		IFunctionDeclaration* func = this->m_Generator.getFunction(name);

		if (func != NULL)
			return TypePosition(true, name);
		else
			return TypePosition(false, false, false, false, 0);
	}
	else
		return this->m_Generator.m_GlobalFrame->getPositionOfVariable(name);
}

// Gets a pointer to the type of a variable based on it's name.
IType* StackFrame::getTypeOfVariable(std::string name)
{
	// Search parameters.
	for (StackMap::iterator i = this->m_ParametersMap.begin(); i != this->m_ParametersMap.end(); i++)
	{
		if ((*i).first == name)
			return (*i).second;
	}

	// Search locals.
	for (StackMap::iterator i = this->m_LocalsMap.begin(); i != this->m_LocalsMap.end(); i++)
	{
		if ((*i).first == name)
			return (*i).second;
	}

	// Search globals.
	if (this->m_Generator.m_GlobalFrame == NULL || this->m_Generator.m_GlobalFrame == this)
	{
		// Also search defined functions.
		IFunctionDeclaration* func = this->m_Generator.getFunction(name);

		if (func != NULL)
			return func->getPointerType();
		else
			return NULL;
	}
	else
		return this->m_Generator.m_GlobalFrame->getTypeOfVariable(name);
}

// Gets the total size of the parameter-portion of the stack frame.
uint16_t StackFrame::getParametersSize()
{
	uint16_t size = 0;

	for (StackMap::iterator i = this->m_ParametersMap.begin(); i != this->m_ParametersMap.end(); i++)
		size += (*i).second->getWordSize(this->m_Generator);

	return size;
}

// Gets the total size of the locals-portion of the stack frame.
uint16_t StackFrame::getLocalsSize()
{
	uint16_t size = 0;

	if (m_LocalsDefined)
	{
		for (StackMap::iterator i = this->m_LocalsMap.begin(); i != this->m_LocalsMap.end(); i++)
			size += (*i).second->getWordSize(this->m_Generator);
	}

	return size;
}
*/