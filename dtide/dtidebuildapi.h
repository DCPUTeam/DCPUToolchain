#ifndef DTIDEBUILDAPI_H
#define DTIDEBUILDAPI_H

#include "backends.h"

class DTIDEBuildAPI: public BuildAPI
{
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
