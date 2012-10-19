#include "dcputoolchain.h"
#include "dcputoolchain_asm.h"
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
    list.push_back("dasm16");
    list.push_back("asm");

    return list;
}

std::string DCPUToolchainASM::GetDefaultFileName()
{
    return "untitled.dasm16";
}

void DCPUToolchainASM::Build(std::string filename, std::string outputDir, BuildAPI &api)
{
    const char* cf = filename.c_str();
    FILE* ob;
    FILE* os;

    if (perform_assemble(cf, &ob, &os))
    {
        std::cout << "Assembling completed succesfully." << std::endl;
        std::cout << "This is where the FILE* handles should be written out" << std::endl
                  << "to disk so that the filenames can be pushed back on" << std::endl
                  << "to the build queue to be processed further." << std::endl;
        fclose(ob);
        fclose(os);
        api.AddOutputFile("CHANGEME.dcpu16");
    }
    else
    {
        std::cout << "Assembling failed!" << std::endl;
    }
    api.End();
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

void DCPUToolchain::Start(std::string path, DebuggingSession& session)
{
    QProcess* dtemu = new QProcess();
    QStringList arguments;

    arguments	<< QString::fromStdString(path);

    dtemu->startDetached("./dtemu", arguments);
}

