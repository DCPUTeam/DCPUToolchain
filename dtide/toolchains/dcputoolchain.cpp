#include "dcputoolchain.h"
#include <iostream> 

DCPUToolchainASM::DCPUToolchainASM() 
{
}

std::string DCPUToolchainASM::GetName()
{
    return "DCPU Toolchain Assembly";
}

std::string DCPUToolchainASM::GetDescription()
{
    return "The DCPU Toolchain Assembly language";
}

std::list<std::string> DCPUToolchainASM::GetExtensions()
{
    std::list<std::string> list;
    list.push_back("dasm");

    return list;
}

std::string DCPUToolchainASM::GetDefaultFileName()
{
    return "untitled.dasm";
}

CodeSyntax DCPUToolchainASM::GetCodeSyntax()
{
    return DCPUAssembly;
}

std::list<std::string> DCPUToolchain::GetAuthors()
{
    std::list<std::string> authors;
    authors.push_back(std::string("The DCPU Toolchain Team"));

    return authors;
}

std::string DCPUToolchain::GetLicense()
{
    return std::string("MIT");
}

std::string DCPUToolchain::GetName()
{
    return "DCPU-16 Toolchain";
}

std::string DCPUToolchain::GetDescription()
{
    return "The DCPU Toolchain.";
}

std::list<Language*> DCPUToolchain::GetLanguages()
{
    std::list<Language*> list;
    DCPUToolchainASM* dasm = new DCPUToolchainASM();

    list.insert(list.end(), dasm);
    return list;
}


