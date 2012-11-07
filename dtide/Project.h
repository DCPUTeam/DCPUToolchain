#ifndef DTIDEPROJECT_H
#define DTIDEPROJECT_H


#include "Backends.h"

class Project: public QObject
{
    Q_OBJECT

public:
    Project();

    virtual bool read(QString file) = 0;

    virtual QString getTitle();
    virtual Toolchain* getToolchain();
    virtual QList<QString> getFileTabs();
    virtual QList<QString> getCompilableFiles();
    virtual QString getRootPath();

    virtual void setTitle(QString title);
    virtual void addFile(QString file, bool compilable);
    virtual void setToolchain(Toolchain* t);

    bool loaded;

protected:
    QString title;
    Toolchain* toolchain;
    QList<QString> fileTabs; 
    QList<QString> compilableFiles;
    QString rootPath;

    Toolchain* resolveToolchain(QString title);
};

#endif
