#include "Toolchain.h"
#include "dtasm.h"
#include "dtemu.h"
#include <iostream>
#include <cstdlib>

#ifdef WIN32
#include <io.h>
#define mktemp _mktemp
#else
#include <unistd.h>
#endif


void DCPUToolchain_CycleHook(vm_t* vm, uint16_t pos, void* ud)
{
}

void DCPUToolchain_WriteHook(vm_t* vm, uint16_t pos, void* ud)
{
    DCPUToolchain* t = static_cast<DCPUToolchain*>(ud);
    DebuggingMessage m;
    MemoryMessage payload;

    payload.pos = pos;
    payload.value = vm->ram[pos];

    m.type = MemoryType;
    m.value = (MessageValue&) payload;

    t->debuggingSession->AddMessage(m);
}

void DCPUToolchain_InterruptHook(vm_t* vm, uint16_t pos, void* ud)
{
    DCPUToolchain* t = static_cast<DCPUToolchain*>(ud);

}

void DCPUToolchain_HardwareHook(vm_t* vm, uint16_t pos, void* ud)
{
    DCPUToolchain* t = static_cast<DCPUToolchain*>(ud);

}

void DCPUToolchain_60HZHook(vm_t* vm, uint16_t pos, void* ud)
{

}



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

void DCPUToolchain::Start(std::string path, DebuggingSession* session)
{
    // Tell the emulator to start.
    debuggingSession = session;
    paused = false;
    start_emulation(
        path.c_str(), 
        &DCPUToolchain_CycleHook, 
        &DCPUToolchain_WriteHook, 
        &DCPUToolchain_InterruptHook,
        &DCPUToolchain_HardwareHook,
        &DCPUToolchain_60HZHook,

        static_cast<void*>(this));
}

void DCPUToolchain::AddStatusMessage(vm_t* vm)
{
    DebuggingMessage m;
    StatusMessage payload;

    if(vm == 0)
        return;

    for(int i = 0; i < 8; i++)
        payload.registers[i] = vm->registers[i];

    payload.pc = vm->pc;
    payload.sp = vm->sp;
    payload.ia = vm->ia;
    payload.ex = vm->ex;

    m.type = StatusType;
    m.value = (MessageValue&) payload;

    debuggingSession->AddMessage(m);
}

void DCPUToolchain::SendStatus()
{
    vm_t* vm = get_vm();

    AddStatusMessage(vm);
}

void DCPUToolchain::Cycle()
{
    if(!paused)
    {
        cycle_emulation();
    }
}

void DCPUToolchain::Step()
{
    cycle_emulation();
}

void DCPUToolchain::Stop(DebuggingSession* session)
{
    stop_emulation();
}

void DCPUToolchain::Pause(DebuggingSession* session)
{
    paused = true;
}

