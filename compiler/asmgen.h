//
// DCPU Compiler by James Rhodes
//
// Assembly generation.
//

#ifndef __DCPU_COMP_ASMGEN_H
#define __DCPU_COMP_ASMGEN_H

class NDeclarations;
class NFunctionDeclaration;

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
};

#endif