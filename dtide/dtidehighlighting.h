#ifndef DTIDEHIGHLIGHTING_H
#define DTIDEHIGHLIGHTING_H

#include "backends.h"
#include "highlighters/highlighters.h"

class DTIDEHighlighting
{
public:
    static QSyntaxHighlighter* getHighlighter(CodeSyntax syntax, QTextDocument* p);

};

#endif
