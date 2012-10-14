#include "dcpuhighlighter.h"

DCPUHighlighter::DCPUHighlighter(QTextDocument* parent): QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    instructionFormat.setForeground(Qt::darkBlue);
    instructionFormat.setFontWeight(QFont::Bold);
    QStringList instructions;

    instructions    << "set" << "add" << "sub"
                    << "mul" << "mli" << "div"
                    << "dvi" << "mod" << "mdi"
                    << "and" << "bor" << "xor"
                    << "shr" << "asr" << "shl"
                    << "ifb" << "ifc" << "ife"
                    << "ifn" << "ifg" << "ifa"
                    << "ifl" << "ifu" << "adx"
                    << "sbx" << "sti" << "std";

    foreach(const QString& instruction, instructions)
    {
        rule.pattern = QRegExp(QString("([ ]+)?") + instruction, Qt::CaseInsensitive);
        rule.format = instructionFormat;

        highlightingRules.append(rule);
    }

    labelFormat.setForeground(Qt::blue);
    rule.pattern = QRegExp("([ ]+)?[a-z0-9_]+:", Qt::CaseInsensitive);
    rule.format = labelFormat;
    highlightingRules.append(rule);

    rule.pattern = QRegExp("([ ]+)?:[a-z0-9_]+");
    highlightingRules.append(rule);

    commentFormat.setFontItalic(true);
    commentFormat.setForeground(Qt::red);
    rule.pattern = QRegExp("([ ]+)?;(.+)", Qt::CaseInsensitive);
    rule.format = commentFormat;
    highlightingRules.append(rule); 
}

void DCPUHighlighter::highlightBlock(const QString& text)
{
    foreach(const HighlightingRule& rule, highlightingRules)
    {
        QRegExp expr(rule.pattern);
        int index = expr.indexIn(text);

        while(index >= 0)
        {
            int length = expr.matchedLength();
            setFormat(index, length, rule.format);
            index = expr.indexIn(text, index + length);
        }
    }
}

