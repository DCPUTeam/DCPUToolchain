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

class Assembler;
class NDeclarations;
class NFunctionDeclaration;
class NType;

#include <cstdint>
#include <exception>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include "node.h"
#include "asmtypes.h"

class AsmBlock
{
private:
	std::string m_Assembly;

    typedef std::basic_ostream<char, std::char_traits<char> > _couttype;
    typedef _couttype& (*_stdendl)(_couttype&);

public:
	AsmBlock();
	
	friend AsmBlock& operator<< (AsmBlock& block, char& input);
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

class StackFrame
{
public:
	typedef std::map<std::string, const NType&> StackMap;
	typedef std::pair<std::string, const NType&> StackPair;

private:
	AsmGenerator& m_Generator;
	StackMap m_StackMap;

public:
	StackFrame(AsmGenerator& generator, StackMap& map) : m_StackMap(map), m_Generator(generator) { };
	int32_t getPositionOfVariable(std::string name);
	uint16_t getSize();
};

class AsmGenerator
{
private:
	const Assembler* m_AssemblerTarget;
	std::vector<std::string> m_AutomaticLabels;
	static char getRandomCharacter();
	static std::string getRandomString(std::string::size_type sz);

public:
	AsmBlock m_Preassembly;
	AsmBlock m_Postassembly;
	StackFrame* m_CurrentFrame;
	NDeclarations* m_RootNode;

public:
	AsmGenerator(std::string asmtarget);
	
	NFunctionDeclaration* getFunction(std::string name);
	StackFrame* generateStackFrame(NFunctionDeclaration* function, bool referenceOnly = true);
	void finishStackFrame(StackFrame* frame);
	size_t getTypeBitSize(const NType& type);
	size_t getTypeWordSize(const NType& type);
	std::string getRandomLabel();
	inline const Assembler& getAssembler() { return *(this->m_AssemblerTarget); }
	inline bool isAssemblerDebug() { return true; }
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
	inline std::string getMessage() { return this->m_Message; }
};

#endif