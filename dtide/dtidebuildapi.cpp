#include "dtidebuildapi.h"

DTIDEBuildAPI::DTIDEBuildAPI()
{
}

void DTIDEBuildAPI::AddError(std::string message, std::string file, int line)
{
    // handle errors
}

void DTIDEBuildAPI::AddWarning(std::string message, std::string file, int line)
{
    // handle warnings
}

void DTIDEBuildAPI::AddIntermediateFile(std::string path, std::string langtarget)
{
    // blah blah intermediate files here
}

void DTIDEBuildAPI::AddOutputFile(std::string path)
{
    output = path;
}

void DTIDEBuildAPI::End()
{
}
