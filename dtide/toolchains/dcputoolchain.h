#ifndef DCPUTOOLCHAIN_H
#define DCPUTOOLCHAIN_H

class DCPUToolchainASM: public Language
{
public:
    std::string Name;
    std::string Description;

    std::list<std::string> Extensions;

    DCPUToolchainASM(): 
        Name(std::string("DCPU Toolchain Assembly")), 
        Description(std::string("The DCPU Toolchain assembly language")), 
        Extensions()
    {
        Extensions.insert(Extensions.end(), "dasm");
    }
    
    void Build(std::string filename, std::string outputDir, BuildAPI& api);
    std::string GetDefaultName()
    {
        return "untitled.dasm";
    }

    CodeSyntax GetCodeSyntax()
    {
        return DCPUAssembly;
    }
};

class DCPUToolchain: public Toolchain
{
public:
    std::string GetName()
    {
        return "DCPU-16 Toolchain";
    }

    std::string GetDescription()
    {
        return "The DCPU Toolchain.";
    }

    std::list<Language*> GetLanguages()
    {
        std::list<Language*> list;
        DCPUToolchainASM* dasm = new DCPUToolchainASM();

        list.insert(list.end(), dasm);
        return list;
    }

    Language* GetLanguageByExtension(std::string ext)
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
};

#endif
