/**

    File:       AsmBlock.h

    Project:    DCPU-16 Tools
    Component:  LibDCPU-ci-lang

    Authors:    James Rhodes

    Description:    Declares the AsmBlock class.

**/

#ifndef __DCPU_LIBDCC_ASMBLOCK_H
#define __DCPU_LIBDCC_ASMBLOCK_H

#include <string>
#include <iostream>
#include <sstream>

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

#endif
