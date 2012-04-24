/**

	File:			CompilerException.h

	Project:		DCPU-16 Tools
	Component:		LibDCC

	Authors:		James Rhodes

	Description:	Declares the CompilerException class.

**/

#ifndef __DCPU_LIBDCC_COMPILEREXCEPTION_H
#define __DCPU_LIBDCC_COMPILEREXCEPTION_H

#include <string>

class CompilerException : public std::exception
{
private:
	std::string m_Message;

public:
	CompilerException(std::string message) : m_Message(message) { };
	virtual ~CompilerException() throw() { };
	virtual const char* what() const throw()
	{
		return this->m_Message.c_str();
	}
	inline std::string getMessage() { return this->m_Message; }
};

#endif