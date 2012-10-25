#include "DCPUHighlighter.h"

#define HEX_COLOR(x, y, z) QBrush(QColor(x, y, z))

DCPUHighlighter::DCPUHighlighter(QTextDocument* parent): QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    instructionFormat.setFontWeight(QFont::Bold);
    instructionFormat.setForeground(HEX_COLOR(0x00, 0x00, 0xff));
    QStringList instructions;

    instructions    << "set" << "add" << "sub"
        		    << "mul" << "mli" << "div"
        		    << "dvi" << "mod" << "mdi"
        		    << "and" << "bor" << "xor"
        		    << "shr" << "asr" << "shl"
        		    << "ifb" << "ifc" << "ife"
        		    << "ifn" << "ifg" << "ifa"
        		    << "ifl" << "ifu" << "adx"
        		    << "sbx" << "sti" << "std"
                    << "jsr" << "int" << "iag"
                    << "ias" << "rfi" << "iaq"
                    << "hwn" << "hwq" << "hwi";

    foreach(const QString& instruction, instructions)
    {
        rule.pattern = QRegExp(QString("([ ]+)?") + instruction, Qt::CaseInsensitive);
        rule.format = instructionFormat;

        highlightingRules.append(rule);
    }

    labelFormat.setFontWeight(QFont::Bold);
    labelFormat.setForeground(HEX_COLOR(0x46, 0x82, 0xb4));
    rule.pattern = QRegExp("([ ]+)?[a-z0-9_]+:", Qt::CaseInsensitive);
    rule.format = labelFormat;
    highlightingRules.append(rule);

    rule.pattern = QRegExp("([ ]+)?:[a-z0-9_]+");
    highlightingRules.append(rule);

    commentFormat.setFontItalic(true);
    commentFormat.setForeground(HEX_COLOR(0x77, 0x88, 0x99));
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

