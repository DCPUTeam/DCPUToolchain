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

#include <cstdint>
#include <exception>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "asmtypes.h"
#include "nodes/NType.h"
#include "nodes/NDeclarations.h"
#include "nodes/NFunctionDeclaration.h"

class TypePosition
{
private:
	bool m_Found;
	bool m_Global;
	bool m_Function;
	uint16_t m_Position;
	std::string m_FunctionName;
public:
	TypePosition(bool isFound, bool isGlobal, uint16_t position);
	TypePosition(bool isFound, std::string funcName);
	bool isFound();
	bool isFunction();
	std::string pushAddress(char registr);
};

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
	typedef std::pair<std::string, NType&> StackPair;
	typedef std::vector<StackPair> StackMap;

private:
	AsmGenerator& m_Generator;
	StackMap m_StackMap;

public:
	StackFrame(AsmGenerator& generator, StackMap& map) : m_StackMap(map), m_Generator(generator) { };
	TypePosition getPositionOfVariable(std::string name);
	NType* getTypeOfVariable(std::string name);
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
	StackFrame* m_GlobalFrame;
	NDeclarations* m_RootNode;

public:
	AsmGenerator(std::string asmtarget);
	
	NFunctionDeclaration* getFunction(std::string name);
	StackFrame* generateStackFrame(NFunctionDeclaration* function, bool referenceOnly = true);
	StackFrame* generateStackFrameIncomplete(NFunctionSignature* signature);
	void finishStackFrame(StackFrame* frame);
	std::string getRandomLabel(std::string prefix);
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