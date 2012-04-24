/**

	File:			StackFrame.cpp

	Project:		DCPU-16 Tools
	Component:		LibDCC

	Authors:		James Rhodes

	Description:	Defines the StackFrame class.

**/

#include "StackFrame.h"

// Gets a relative stack position by the variable name.
TypePosition StackFrame::getPositionOfVariable(std::string name, bool stackStartAtC)
{
	uint16_t size = 0;
	for (StackMap::iterator i = this->m_StackMap.begin(); i != this->m_StackMap.end(); i++)
	{
		if ((*i).first == name)
			return TypePosition(true, this->m_Generator.m_GlobalFrame == this, stackStartAtC, size);
		else
			size += (*i).second.getWordSize(this->m_Generator);
	}
	if (this->m_Generator.m_GlobalFrame == NULL || this->m_Generator.m_GlobalFrame == this)
	{
		// Also search defined functions.
		IFunctionDeclaration* func = this->m_Generator.getFunction(name);
		if (func != NULL)
			return TypePosition(true, name);
		else
			return TypePosition(false, false, false, 0);
	}
	else
		return this->m_Generator.m_GlobalFrame->getPositionOfVariable(name);
}

// Gets a pointer to the type of a variable based on it's name.
IType* StackFrame::getTypeOfVariable(std::string name)
{
	for (StackMap::iterator i = this->m_StackMap.begin(); i != this->m_StackMap.end(); i++)
	{
		if ((*i).first == name)
			return &((*i).second);
	}
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

// Gets the total size of the stack frame.
uint16_t StackFrame::getSize()
{
	uint16_t size = 0;
	for (StackMap::iterator i = this->m_StackMap.begin(); i != this->m_StackMap.end(); i++)		
		size += (*i).second.getWordSize(this->m_Generator);
	return size;
}