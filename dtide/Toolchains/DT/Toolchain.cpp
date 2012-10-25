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

DCPUToolchain* g_this;

void* DCPUToolchain_CreateContext(const char* title, int width, int height, bool resizeable, void* ud)
{
    QString qtitle = QString::fromLocal8Bit(title);
    QGLWidget* context = g_this->gl->requestWidget(qtitle, width, height);
    return context;
}

void DCPUToolchain_ActivateContext(void* context)
{
    QGLWidget* w = static_cast<QGLWidget*>(context);
    w->makeCurrent();
}

void DCPUToolchain_SwapBuffers(void* context)
{
    QGLWidget* w = static_cast<QGLWidget*>(context);
    w->swapBuffers();
} 

void DCPUToolchain_DestroyContext(void* context)
{
    // FIXME
}

void* DCPUToolchain_GetUD(void* context)
{
    return NULL;
}

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
    // For the lack of a proper solution...
    g_this = this;

    // Tell the emulator to start.
    debuggingSession = session;
    paused = false;
    start_emulation(
        /* Binary path */
        path.c_str(), 

        /* VM Hooks */
        &DCPUToolchain_CycleHook, 
        &DCPUToolchain_WriteHook, 
        &DCPUToolchain_InterruptHook,
        &DCPUToolchain_HardwareHook,
        &DCPUToolchain_60HZHook,

        /* Host context */
        &DCPUToolchain_CreateContext,
        &DCPUToolchain_DestroyContext,
        &DCPUToolchain_ActivateContext,
        &DCPUToolchain_SwapBuffers,
        &DCPUToolchain_GetUD,

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
    gl->killWidgets();
    stop_emulation();
}

void DCPUToolchain::Pause(DebuggingSession* session)
{
    paused = true;
}

