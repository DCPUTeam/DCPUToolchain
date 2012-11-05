#include "DTIDEBuildAPI.h"

#ifndef WIN32
#include <unistd.h>
#endif

DTIDEBuildAPI::DTIDEBuildAPI()
{
}

DTIDEBuildAPI::~DTIDEBuildAPI()
{
    std::list<std::string>::iterator it;

    // Clean up.
    for(it = m_OutputFiles.begin(); it != m_OutputFiles.end(); it++)
        unlink((*it).c_str());

    for(it = m_SymbolFiles.begin(); it != m_SymbolFiles.begin(); it++)
        unlink((*it).c_str());
}

void DTIDEBuildAPI::AddError(std::string message, std::string file, int line)
{
    ErrorEntry entry;
    entry.IsWarning = false;
    entry.Message = message;
    entry.File = file;
    entry.Line = line;
    m_ErrorEntries.insert(m_ErrorEntries.begin(), entry);
}

void DTIDEBuildAPI::AddWarning(std::string message, std::string file, int line)
{
    ErrorEntry entry;
    entry.IsWarning = true;
    entry.Message = message;
    entry.File = file;
    entry.Line = line;
    m_ErrorEntries.insert(m_ErrorEntries.begin(), entry);
}

void DTIDEBuildAPI::AddIntermediateFile(std::string path, std::string langtarget)
{
    IntermediateFile entry;
    entry.Path = path;
    entry.Language = langtarget;
    m_IntermediateFiles.insert(m_IntermediateFiles.begin(), entry);
}

void DTIDEBuildAPI::AddOutputFile(std::string path)
{
    m_OutputFiles.insert(m_OutputFiles.begin(), path);
}

void DTIDEBuildAPI::AddSymbolsFile(std::string path)
{
    m_SymbolFiles.insert(m_SymbolFiles.begin(), path);
}

void DTIDEBuildAPI::End()
{
}
