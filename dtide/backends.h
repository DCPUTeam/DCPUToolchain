#ifndef BACKENDS_H
#define BACKENDS_H

#include "highlighters/highlighters.h"

struct ProjectProperties
{
    int type;
    QString fileName;
};


class DTIDEBackends
{
public:
    enum Types
    {
        DCPU_ASSEMBLY,
        DCPU_TOOLCHAIN_C,
        DCPU_BRAINFUCK
    };

    static QSyntaxHighlighter* getHighlighter(int type, QTextDocument* parent);
    static ProjectProperties getUntitledProperties(int type);
};

#endif
