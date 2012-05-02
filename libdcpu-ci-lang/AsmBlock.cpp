/**

	File:		AsmBlock.cpp

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang

	Authors:	James Rhodes

	Description:	Defines the AsmBlock class.

**/

#include <fstream>
#include <iterator>
#include "AsmBlock.h"

AsmBlock::AsmBlock()
{
}

AsmBlock& operator<< (AsmBlock& block, char& input)
{
	std::stringstream sstr;
	sstr << input;
	return block << sstr.str();
}

AsmBlock& operator<< (AsmBlock& block, const size_t& input)
{
	std::stringstream sstr;
	sstr << input;
	return block << sstr.str();
}

AsmBlock& operator<< (AsmBlock& block, const char input[])
{
	return block << std::string(input);
}

AsmBlock& operator<< (AsmBlock& block, std::ifstream& input)
{
	std::string data((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
	block.m_Assembly += data;
	return block;
}

AsmBlock& operator<< (AsmBlock& block, const std::string& input)
{
	block.m_Assembly += input;
	return block;
}

AsmBlock& operator<< (AsmBlock& block, const AsmBlock& input)
{
	if (&input == NULL) return block;

	block.m_Assembly += input.m_Assembly + "\n";
	return block;
}

std::ostream& operator<< (std::ostream& output, const AsmBlock& block)
{
	output << block.m_Assembly;
	return output;
}
