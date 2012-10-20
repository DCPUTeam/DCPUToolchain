#include "DTIDEBuildAPI.h"

DTIDEBuildAPI::DTIDEBuildAPI()
{
}

void DTIDEBuildAPI::AddError(std::string message, std::string file, int line)
{
    ErrorEntry entry;
    entry.IsWarning = false;
    entry.Message = message;
    entry.File = file;
    entry.Line = line;
    this->m_ErrorEntries.insert(this->m_ErrorEntries.begin(), entry);
}

void DTIDEBuildAPI::AddWarning(std::string message, std::string file, int line)
{
    ErrorEntry entry;
    entry.IsWarning = true;
    entry.Message = message;
    entry.File = file;
    entry.Line = line;
    this->m_ErrorEntries.insert(this->m_ErrorEntries.begin(), entry);
}

void DTIDEBuildAPI::AddIntermediateFile(std::string path, std::string langtarget)
{
    IntermediateFile entry;
    entry.Path = path;
    entry.Language = langtarget;
    this->m_IntermediateFiles.insert(this->m_IntermediateFiles.begin(), entry);
}

void DTIDEBuildAPI::AddOutputFile(std::string path)
{
    this->m_OutputFiles.insert(this->m_OutputFiles.begin(), path);

    // FIXME: Remove this when we have proper project support.
    output = path;
}

void DTIDEBuildAPI::End()
{
}
