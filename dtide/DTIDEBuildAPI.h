#ifndef DTIDEBUILDAPI_H
#define DTIDEBUILDAPI_H

#include <list>
#include <string>
#include "Backends.h"

class DTIDEBuildAPI: public BuildAPI
{
public:
    DTIDEBuildAPI();

    void AddError(std::string message, std::string file, int line);
    void AddWarning(std::string message, std::string file, int line);

    void AddIntermediateFile(std::string path, std::string langtarget);
    void AddOutputFile(std::string path);
    void AddSymbolsFile(std::string path);

    void End();

};

#endif
