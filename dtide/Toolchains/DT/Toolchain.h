#ifndef DCPUTOOLCHAIN_H
#define DCPUTOOLCHAIN_H

#include <string>
#include <list>
#include "../../Backends.h"

#include <QDebug>
#include <QProcess>
#include <QStringList>

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

    virtual std::list<Language*> GetLanguages();

    virtual void Start(std::string path, DebuggingSession* session);
    virtual void Stop(DebuggingSession& session);

    DebuggingSession* debuggingSession;
};

#endif
