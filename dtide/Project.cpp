#include "Project.h"

Project::Project(): loaded(false)
{
    loaded = false;
}

Toolchain* Project::resolveToolchain(QString title)
{
    return new DCPUToolchain();
}

QString Project::getTitle()
{
    return title;
}

Toolchain* Project::getToolchain()
{
    return toolchain;
}

QList<QString> Project::getFileTabs()
{
    return fileTabs;
}

QList<QString> Project::getCompilableFiles()
{
    return compilableFiles;
}

QString Project::getRootPath()
{
    return rootPath;
}

void Project::setTitle(QString a_title)
{
    title = a_title;
}

void Project::addFile(QString file, bool compilable)
{
    loaded = true;
    
    fileTabs.append(file);
    if(compilable)
        compilableFiles.append(file);
}

void Project::setToolchain(Toolchain* t)
{
    toolchain = t;
}
