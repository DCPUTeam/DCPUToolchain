#ifndef DCPUHIGHLIGHTER_H
#define DCPUHIGHLIGHTER_H

#include <QSyntaxHighlighter>

class DCPUHighlighter: public QSyntaxHighlighter
{
    Q_OBJECT

public:
    DCPUHighlighter(QTextDocument* parent = 0);

protected:
    void highlightBlock(const QString& text);

private:
    struct HighlightingRule
    {
	QRegExp pattern;
	QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QTextCharFormat instructionFormat;
    QTextCharFormat commentFormat;
    QTextCharFormat labelFormat;

};

#endif
