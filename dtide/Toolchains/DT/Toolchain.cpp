#include "Toolchain.h"
#include "dtasm.h"
#include "dtemu.h"
#include "dtdb.h"
#include <iostream>
#include <cstdlib>
#include <cassert>

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
    DTIDEGLWidget* context = g_this->gl->requestWidget(g_this, qtitle, width, height, ud);
    return (void*) context;
}

void DCPUToolchain_ActivateContext(void* context)
{
    QGLWidget* w = static_cast<DTIDEGLWidget*>(context);
    w->makeCurrent();
}

void DCPUToolchain_SwapBuffers(void* context)
{
    QGLWidget* w = static_cast<DTIDEGLWidget*>(context);
    w->makeCurrent();
    w->swapBuffers();
    w->doneCurrent();
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
    DCPUToolchain* t = static_cast<DCPUToolchain*>(ud);

    Line line = t->LineAt(t->debuggingSession, vm->pc);
    if(line.LineNumber != 0 && !vm->skip) {
        t->debuggingSession->SetCurrentLine(line);
    }

    // Check for breakpoints
    if(t->debuggingSession->BreakpointAt(vm->pc))
    {
        t->Pause(t->debuggingSession);
    }
}

void DCPUToolchain_WriteHook(vm_t* vm, uint16_t pos, void* ud)
{
    DCPUToolchain* t = static_cast<DCPUToolchain*>(ud);
    t->debuggingSession->RegisterMemoryChange(pos, vm->ram[pos]);
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

void DCPUToolchainASM::Build(std::string filename, std::string outputDir, BuildAPI& api)
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
        std::cout << "Assembling success!" << std::endl;
        api.AddSymbolsFile(os);
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
    return "DCPU Toolchain";
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


void DCPUToolchain::RelayResize(int w, int h, void* ud)
{
    lem_resize(ud, w, h);
}

void DCPUToolchain::Start(BuildAPI& result, DebuggingSession* session)
{
    // For the lack of a proper solution...
    g_this = this;

    // Tell the emulator to start.
    debuggingSession = session;
    paused = false;

    // Get the output files.
    std::list<std::string> outputFiles = result.GetOutputFiles();

    // Change this when we have project support.
    assert(outputFiles.size() == 1);
    std::string path(*(outputFiles.begin()));
    

    vm = start_emulation(
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
    
    // set initial memory configuration to debuggingSession
    debuggingSession->SetMemory(vm->ram);

    // Load debugging symbols
    std::list<std::string> symbolFiles = result.GetSymbolFiles();

    // Change this when we have project support.
    assert(symbolFiles.size() == 1);

    // Load the symbols.
    std::string symbolsPath(*(symbolFiles.begin()));
    dtdb_read_symbols(symbolsPath.c_str());
}

void DCPUToolchain::AddStatusMessage(vm_t* vm)
{
    DebuggingMessage m;
    StatusMessage payload;

    if (vm == 0)
        return;

    for (int i = 0; i < 8; i++)
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
    if (!paused)
    {
        cycle_emulation();
    }
}

void DCPUToolchain::Step()
{
    vm->sleep_cycles = 0;
    cycle_emulation();
}

void DCPUToolchain::Stop(DebuggingSession* session)
{
    gl->killWidgets();
    paused = true;
    stop_emulation();
}

void DCPUToolchain::Pause(DebuggingSession* session)
{
    paused = true;
}

void DCPUToolchain::Resume(DebuggingSession* session)
{
    paused = false;
}

void DCPUToolchain::AddBreakpoint(DebuggingSession* session, Breakpoint& b)
{
    int32_t address = dtdb_get_line_address(b.File.c_str(), b.Line);
    if(address != -1)
        session->AddBreakpoint((uint16_t) address);
    else
        std::cout << "Unable to resolve breakpoint " << b.File << ":" << b.Line << std::endl;
}

Line DCPUToolchain::LineAt(DebuggingSession* session, uint16_t address)
{
    Line result;

    result.LineNumber = 0;
    result.Path = NULL;

    if(dtdb_line_at_address(address))
    {
        result.LineNumber = dtdb_get_line_number();
        result.Path = dtdb_get_line_path();
    }

    return result;
}
