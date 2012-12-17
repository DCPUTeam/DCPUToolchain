#ifndef DCPUTOOLCHAIN_H
#define DCPUTOOLCHAIN_H

#include <string>
#include <list>
#include "../../Backends.h"
#include "../../DTIDEGLWidget.h"

#include <QDebug>
#include <QGLWidget>
#include <QGLFormat>

extern "C" {
#include <vm.h>
}

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

class DCPUToolchain : public Toolchain
{
public:
    virtual std::string GetName();
    virtual std::string GetDescription();
    virtual std::list<std::string> GetAuthors();
    virtual std::string GetLicense();
    virtual void Cycle();
    virtual void Step();

    virtual std::list<Language*> GetLanguages();

    virtual void Start(BuildAPI& result, DebuggingSession* session);
    virtual void Stop(DebuggingSession* session);
    virtual void Pause(DebuggingSession* session);
    virtual void Resume(DebuggingSession* session);
    virtual Line LineAt(DebuggingSession* session, uint16_t address);
    
    virtual void RelayResize(int w, int h, void* ud);

    virtual void SendStatus();

    DebuggingSession* debuggingSession;

    virtual void AddBreakpoint(DebuggingSession* session, Breakpoint& b);
    
    vm_t* vm;
private:
    

    void AddStatusMessage(vm_t* vm);
};

#endif
