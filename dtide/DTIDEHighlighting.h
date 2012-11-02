#ifndef DTIDEHIGHLIGHTING_H
#define DTIDEHIGHLIGHTING_H

#include <qscilexercustom.h>
#include "Backends.h"
#include "Highlighters/Highlighters.h"

class DTIDEHighlighting
{
public:
    static QsciLexer* getHighlighter(CodeSyntax syntax, QObject* p);
};

#endif
