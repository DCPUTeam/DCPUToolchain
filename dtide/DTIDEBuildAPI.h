#ifndef DTIDEBUILDAPI_H
#define DTIDEBUILDAPI_H

#include <list>
#include <string>
#include "Backends.h"

class DTIDEBuildAPI: public BuildAPI
{
private:
    struct IntermediateFile
    {
        std::string Path;
        std::string Language;
    };

    struct ErrorEntry
    {
        bool IsWarning;
        std::string Message;
        std::string File;
        int Line;
    };

    std::list<IntermediateFile> m_IntermediateFiles;
    std::list<std::string> m_OutputFiles;
    std::list<ErrorEntry> m_ErrorEntries;

public:
    DTIDEBuildAPI();

    void AddError(std::string message, std::string file, int line);
    void AddWarning(std::string message, std::string file, int line);

    void AddIntermediateFile(std::string path, std::string langtarget);
    void AddOutputFile(std::string path);

    void End();

    std::string output;
};

#endif
