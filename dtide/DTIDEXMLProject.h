#ifndef DTIDEXML_PROJECT_H
#define DTIDEXML_PROJECT_H

#include "Project.h"
#include <QObject>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QFile>
#include <QFileInfo>

class DTIDEXMLProject: public Project
{
public:
    DTIDEXMLProject();

    virtual bool read(QString file);
};

#endif
