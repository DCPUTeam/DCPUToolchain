/**

	File:           TypePosition.cpp

	Project:        DCPU-16 Tools
	Component:      LibDCPU-ci-lang

	Authors:        James Rhodes
	                Patrick Flick

	Description:    Defines the TypePosition class.

**/

#include <sstream>
#include "TypePosition.h"
#include "CompilerException.h"

TypePosition::TypePosition(bool isFound, bool isGlobal, bool hasStackStartAtC, uint16_t position)
{
	this->m_Found = isFound;
	this->m_Function = false;
	this->m_FunctionName = "";
	this->m_Global = isGlobal;
	this->m_StackStartAtC = hasStackStartAtC;
	this->m_Position = position;
}

TypePosition::TypePosition(bool isFound, std::string funcName)
{
	this->m_Found = true;
	this->m_Function = true;
	this->m_FunctionName = funcName;
	this->m_Global = true;
	this->m_StackStartAtC = false;
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

std::string TypePosition::pushAddress(char registr)
{
	std::stringstream sstr;
	if (!this->m_Found)
		throw new CompilerException("Attempted to push reference position of unknown type position result (internal error).");
	if (this->m_Function)
		sstr << "	SET " << registr << ", cfunc_" << this->m_FunctionName << std::endl;
	else if (this->m_Global)
		sstr << "	SET " << registr << ", _DATA" << std::endl;
		
	// TODO rename the member m_StackStartAtC, it now works differently
	else if (this->m_StackStartAtC)
	{	/*
		sstr << "	SET " << registr << ", C" << std::endl;
		sstr << "	ADD " << registr << ", 1" << std::endl;
		*/
		// there is one stack-frame allready in lower memory
		// thus get the address of the next stack via Y (iterate one position down)
		sstr << "	SET " << registr << ", Y" << std::endl;
		sstr << "	SUB " << registr << ", 1" << std::endl;
		sstr << "	SET " << registr << ", [" << registr << "]" << std::endl;
	}
	else
		sstr << "	SET " << registr << ", Y" << std::endl;
	if (this->m_Position != 0)
		sstr << "	ADD " << registr << ", " << this->m_Position << std::endl;
	return sstr.str();
}
