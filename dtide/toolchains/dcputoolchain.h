#ifndef DCPUTOOLCHAIN_H
#define DCPUTOOLCHAIN_H

#include <string>
#include <list>
#include "../backends.h"


class DCPUToolchainASM: public Language
{
public:
    std::string GetName();
    std::string GetDescription();
    std::list<std::string> GetExtensions();

    DCPUToolchainASM();
    
    void Build(std::string filename, std::string outputDir, BuildAPI& api);
    std::string GetDefaultFileName();

    CodeSyntax GetCodeSyntax();
};

class DCPUToolchain: public Toolchain
{
public:
    std::string GetName();

    std::string GetDescription();

    std::list<std::string> GetAuthors();
    std::string GetLicense();

    std::list<Language*> GetLanguages();
};

#endif
