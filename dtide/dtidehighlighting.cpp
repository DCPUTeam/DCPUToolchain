#include "dtidehighlighting.h"

QSyntaxHighlighter* DTIDEHighlighting::getHighlighter(CodeSyntax syntax, QTextDocument* p)
{
    switch(syntax)
    {
        case DCPUAssembly:
            return new DCPUHighlighter(p);
            break;
    }
}
