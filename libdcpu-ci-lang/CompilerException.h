/**

	File:		CompilerException.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang

	Authors:	James Rhodes

	Description:	Declares the CompilerException class.

**/

#ifndef __DCPU_LIBDCC_COMPILEREXCEPTION_H
#define __DCPU_LIBDCC_COMPILEREXCEPTION_H

#include <string>
#include <sstream>

class CompilerException : public std::exception
{
	private:
		int m_Line;
		std::string m_File;
		std::string m_Message;

	public:
		CompilerException(int line, std::string file, std::string message) : m_Line(line), m_File(file), m_Message(message) { };
		virtual ~CompilerException() throw() { };
		virtual const char* what() const throw()
		{
			std::stringstream combined;
			combined << this->m_Line << ":" << this->m_File << ": " << this->m_Message;
			return combined.str().c_str();
		}
		inline std::string getMessage()
		{
			std::stringstream combined;
			combined << this->m_Line << ":" << this->m_File << ": " << this->m_Message;
			return combined.str();
		}
};

#endif
