#include "DTIDEXMLProject.h"

DTIDEXMLProject::DTIDEXMLProject(): Project()
{
}

bool DTIDEXMLProject::read(QString f)
{
    QDomDocument doc("project");
    QFile file(f);
    if (!file.open(QIODevice::ReadOnly))
        return false;
    if (!doc.setContent(&file)) {
        file.close();
        return false;
    }
    QFileInfo info(file);
    rootPath = info.absolutePath();
    file.close();
    
    QDomElement docElem = doc.documentElement();
    
    QDomNode n = docElem.firstChild();
    while(!n.isNull()) {
        QDomElement e = n.toElement(); 
        if(!e.isNull()) {
            if(e.tagName() == "title")
                title = e.text();
            if(e.tagName() == "toolchain")
                toolchain = resolveToolchain(e.text());
            if(e.tagName() == "files")
            {
                QDomNode fn = e.firstChild();
                QDomElement fe = e.firstChild().toElement();
                while(!fn.isNull())
                {
                    fe = fn.toElement();
                    if(!fe.isNull())
                    {
                        if(fe.tagName() == "file")
                        {
                            fileTabs.append(fe.text());
                            if(fe.hasAttribute("compile"))
                                if(fe.attribute("compile") == "true")
                                    compilableFiles.append(fe.text());
                        }
                    }
                    fn = fn.nextSibling();
                }
            }
        }
        n = n.nextSibling();
    }

    loaded = true;
    return true;
}
