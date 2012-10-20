#include "Toolchain.h"
#include "dtasm.h"
#include "dtemu.h"
#include <iostream>
#include <cstdlib>

#ifdef WIN32
#include <io.h>
#define mktemp _mktemp
#endif

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
    const char* t_ob = "dtcpu.XXXXXX";
    const char* t_os = "dtsym.XXXXXX";
    char* ob = (char*)malloc(strlen(t_ob) + 1);
    char* os = (char*)malloc(strlen(t_os) + 1);
    memcpy(ob, t_ob, strlen(t_ob) + 1);
    memcpy(os, t_os, strlen(t_os) + 1);
    // FIXME: Create a function in osutil for creating a temporary filename.
    // FIXME: Create a global "temporary file" manager that cleans up files
    //        that were created during builds when the IDE exits.
    mktemp(ob);
    mktemp(os);

    if (perform_assemble(cf, ob, os))
    {
        api.AddOutputFile(ob);
        unlink(os);
        std::cout << "Assembling success!" << std::endl;
    }
    else
    {
        unlink(ob);
        unlink(os);
        std::cout << "Assembling failed!" << std::endl;
    }
    api.End();

    free(ob);
    free(os);
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
    // Tell the emulator to start.
    start_emulation(path.c_str());
}

void DCPUToolchain::Cycle()
{
    cycle_emulation();
}

void DCPUToolchain::Stop(DebuggingSession& session)
{
    stop_emulation();
}
