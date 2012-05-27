/**

	File:		TypePosition.cpp

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang

	Authors:	James Rhodes
			Patrick Flick

	Description:	Defines the TypePosition class.

**/

#include <sstream>
#include "TypePosition.h"
#include "CompilerException.h"

TypePosition::TypePosition(bool isFound, bool isGlobal, bool isFunctionParameter, bool previousStackFrame, uint16_t position)
{
	this->m_Found = isFound;
	this->m_Function = false;
	this->m_FunctionName = "";
	this->m_Global = isGlobal;
	this->m_IsFunctionParameter = isFunctionParameter;
	this->m_PreviousStackFrame = previousStackFrame;
	this->m_Position = position;
}

TypePosition::TypePosition(bool isFound, std::string funcName)
{
	this->m_Found = true;
	this->m_Function = true;
	this->m_FunctionName = funcName;
	this->m_Global = true;
	this->m_IsFunctionParameter = false;
	this->m_PreviousStackFrame = false;
	this->m_Position = 0;
}

bool TypePosition::isFound()
{
	return this->m_Found;
}

bool TypePosition::isFunction()
{
	return this->m_Function;
}

std::string TypePosition::getAddress()
{
	std::stringstream sstr;

	if (!this->m_Found)
		throw new CompilerException(0, "<internal>", "Attempted to get reference position of unknown type position result (internal error).");
	
	if (this->m_Function)
		sstr << "cfunc_" << this->m_FunctionName;
	else if (this->m_Global)
		sstr << "_DATA+" << this->m_Position;
	else if (this->m_PreviousStackFrame && this->m_IsFunctionParameter)
		sstr << "[Y+1]+" << this->m_Position;
	else if (this->m_PreviousStackFrame)
		sstr << "[Y+1]+" << (0x10000 - (this->m_Position + 2));
	else if (this->m_IsFunctionParameter)
		sstr << "Y+" << this->m_Position;
	else
		sstr << "Y+" << (0x10000 - (this->m_Position + 2));

	return sstr.str();
}

std::string TypePosition::pushAddress(char registr)
{
	std::stringstream sstr;

	if (!this->m_Found)
		throw new CompilerException(0, "<internal>", "Attempted to push reference position of unknown type position result (internal error).");

	if (this->m_Function)
		sstr << "	SET " << registr << ", cfunc_" << this->m_FunctionName << std::endl;
	else if (this->m_Global)
	{
		sstr << "	SET " << registr << ", _DATA" << std::endl;
		if (this->m_Position != 0)
			// we use the positions from here on _downwards_
			sstr << "	ADD " << registr << ", " << this->m_Position << std::endl;
	}
	else
	{
		// two more cases:
		//  local variables: is in the Stack in Position Y-3-(relative local position)
		//  function parameter: in Stack in Position Y+(relative local position)

		// get current stack frame address
		sstr << "	SET " << registr << ", Y" << std::endl;

		// do we want to reference the previous stack frame?
		if (this->m_PreviousStackFrame)
		{
			sstr << "	SUB " << registr << ", 1" << std::endl;
			sstr << "	SET " << registr << ", [" << registr << "]" << std::endl;
		}


		if (this->m_IsFunctionParameter)
		{
			// function parameters in stack position Y+pos
			if (this->m_Position != 0)
				sstr << "	ADD " << registr << ", " << this->m_Position << std::endl;
		}
		else
		{
			// locals are in Y-3-pos
			// jumping over next frame pointer and return address
			sstr << "	SUB " << registr << ", 2" << std::endl;
			if (this->m_Position != 0)
				sstr << "	SUB " << registr << ", " << this->m_Position << std::endl;
		}
	}

	return sstr.str();
}
