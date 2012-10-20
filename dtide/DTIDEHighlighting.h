#ifndef DTIDEHIGHLIGHTING_H
#define DTIDEHIGHLIGHTING_H

#include "Backends.h"
#include "Highlighters/Highlighters.h"

class DTIDEHighlighting
{
public:
    static QSyntaxHighlighter* getHighlighter(CodeSyntax syntax, QTextDocument* p);
};

#endif
