#ifndef DCPUTOOLCHAIN_H
#define DCPUTOOLCHAIN_H

#include <string>
#include <list>
#include "../backends.h"


class DCPUToolchainASM: public Language
{
public:
    std::string Name;
    std::string Description;

    std::list<std::string> Extensions;

    DCPUToolchainASM();
    
    void Build(std::string filename, std::string outputDir, BuildAPI& api);
    std::string GetDefaultName();

    CodeSyntax GetCodeSyntax();
};

class DCPUToolchain: public Toolchain
{
public:
    std::string GetName();

    std::string GetDescription();

    std::list<Language*> GetLanguages();

    Language* GetLanguageByExtension(std::string ext);
};

#endif
