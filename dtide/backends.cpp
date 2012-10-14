#include "backends.h"

QSyntaxHighlighter* DTIDEBackends::getHighlighter(int type, QTextDocument* parent)
{
    switch(type)
    {
        case DCPU_ASSEMBLY:
            return new DCPUHighlighter(parent);
            break;
    }
}

ProjectProperties DTIDEBackends::getUntitledProperties(int type)
{
    ProjectProperties p;
    p.type = type;

    switch(type)
    {
        case DCPU_ASSEMBLY:
            p.fileName = "untitled.dasm";
            break;
    }

    return p;
}
