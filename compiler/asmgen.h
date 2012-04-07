/**

	File:			asmgen.h

	Project:		DCPU-16 Tools
	Component:		Compiler

	Authors:		James Rhodes

	Description:	Declares the classes used to assist AST
					nodes in generating assembly.

**/

#ifndef __DCPU_COMP_ASMGEN_H
#define __DCPU_COMP_ASMGEN_H

class NDeclarations;
class NFunctionDeclaration;
class NType;

#include <exception>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include "node.h"

class AsmBlock
{
private:
	std::string m_Assembly;

    typedef std::basic_ostream<char, std::char_traits<char> > _couttype;
    typedef _couttype& (*_stdendl)(_couttype&);

public:
	AsmBlock();
	
	friend AsmBlock& operator<< (AsmBlock& block, const size_t& input);
	friend AsmBlock& operator<< (AsmBlock& block, const char input[]);
	friend AsmBlock& operator<< (AsmBlock& block, std::ifstream& input);
	friend AsmBlock& operator<< (AsmBlock& block, const std::string& input);
	friend AsmBlock& operator<< (AsmBlock& block, const AsmBlock& input);
	friend std::ostream& operator<< (std::ostream& output, const AsmBlock& block);
    AsmBlock& operator<<(_stdendl manip)
    {
		std::stringstream sstr;
		sstr << std::endl;
		this->m_Assembly += sstr.str();
        return *this;
    }
};

class AsmGenerator
{
public:
	AsmBlock m_Preassembly;
	AsmBlock m_Postassembly;

public:
	AsmGenerator();
	
	NFunctionDeclaration* getMainFunction(NDeclarations* function);
	size_t getStackSize(NFunctionDeclaration* function);
	size_t getTypeSize(const NType& type);
};

class CompilerException : public std::exception
{
private:
	std::string m_Message;

public:
	CompilerException(std::string message) : m_Message(message) { };
	virtual const char* what() const throw()
	{
		return this->m_Message.c_str();
	}
};

#endif