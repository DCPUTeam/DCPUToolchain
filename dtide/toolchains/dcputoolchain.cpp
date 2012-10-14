#include "dcputoolchain.h"

    DCPUToolchainASM::DCPUToolchainASM(): 
        Name(std::string("DCPU Toolchain Assembly")), 
        Description(std::string("The DCPU Toolchain assembly language")), 
        Extensions()
    {
        Extensions.insert(Extensions.end(), "dasm");
    }
    
    std::string DCPUToolchainASM::GetDefaultName()
    {
        return "untitled.dasm";
    }
    
    CodeSyntax DCPUToolchainASM::GetCodeSyntax()
    {
        return DCPUAssembly;
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
    
    Language* DCPUToolchain::GetLanguageByExtension(std::string ext)
    {
        std::list<Language*>::iterator it;
        std::list<Language*> langs = GetLanguages();
        for(it = langs.begin(); it != langs.end(); it++)
        {
            std::list<std::string> exts = dynamic_cast<Language*>(*it)->Extensions;
            std::list<std::string>::iterator it_ext;

            for(it_ext = exts.begin(); it_ext != exts.end(); it_ext++)
            {
                if(*it_ext == ext)
                    return *it;
            }
        }
    }

