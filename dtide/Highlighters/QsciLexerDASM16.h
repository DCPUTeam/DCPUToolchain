#ifndef QSCILEXERASM_H
#define QSCILEXERASM_H

#include <QObject>

#include <qsciglobal.h>
#include <qscilexer.h>

class QsciLexerDASM16:public QsciLexer
{
    Q_OBJECT
public:
    //Style enum for the ASM lexer.
    enum
    {
        Default=0,
        Comment=1,
        Number=2,
        DoubleQuotedString=3,
        Operator=4,
        Identifier=5,
        DCPU_Instruction=6,
        Register=8,
        DASM_Directive=9,
        DASM_Directive_Operand=10,
        Comment_Block=11,           //Not implimented..
        SingleQuoteString=12,
        UnclosedString=13,
        Ext_Instruction=14
    };
    QsciLexerDASM16(QObject *parent=0, bool caseInsensitiveKeywords=false);
    virtual ~QsciLexerDASM16();
    const char *language() const;
    const char *lexer() const;
    
    QStringList autoCompletionWordSeparators() const;

    const char *wordCharacters() const;
    QColor defaultColor(int style) const;
    bool defaultEolFill(int style) const;
    QFont defaultFont(int style) const;
    QColor defaultPaper(int style) const;
    const char *keywords(int set) const;
    QString description(int style) const;

private:
    bool dollars;
    bool nocase;

    void setDollarsProp();

    QsciLexerDASM16(const QsciLexerDASM16 &);
    QsciLexerDASM16 &operator=(const QsciLexerDASM16 &);
};

#endif // QSCILEXERASM_H
