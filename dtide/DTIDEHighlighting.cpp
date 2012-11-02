#include "DTIDEHighlighting.h"

QsciLexer* DTIDEHighlighting::getHighlighter(CodeSyntax syntax, QObject* p)
{
    switch (syntax)
    {
        case DCPUAssembly:
            return new QsciLexerDASM16(p);
        case Brainfuck:
            return NULL;
        case C:
            return NULL;
        default:
            return NULL;
    }
}
