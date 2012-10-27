#include "QsciLexerDASM16.h"

#include <qcolor.h>
#include <qfont.h>
#include <qsettings.h>
#include <stdio.h>


QsciLexerDASM16::QsciLexerDASM16(QObject *parent, bool caseInsensitiveKeywords): QsciLexer(parent), nocase(caseInsensitiveKeywords)
{
}

QsciLexerDASM16::~QsciLexerDASM16() {}


const char* QsciLexerDASM16::language() const
{
    return "asm";
}


const char *QsciLexerDASM16::lexer() const
{
    return "asm";
}

QStringList QsciLexerDASM16::autoCompletionWordSeparators() const
{
    QStringList list;
    list << "set pc, " << ".";
    return list;
}

const char* QsciLexerDASM16::blockStartKeyword(int *style) const
{
    if(style)
        *style = DASM_Directive;

    return ".if .else .elseif .macro macro .define .equ";
}

const char* QsciLexerDASM16::blockEndKeyword(int *style) const
{
    if(style)
        *style = DASM_Directive;

    return ".endif";
}

const char* QsciLexerDASM16::blockEnd(int *style) const
{
    if(style)
        *style = DCPU_Instruction;

    return "set pc, pop";
}

const char* QsciLexerDASM16::blockStart(int *style) const
{
    return ":"; // Label
}

const char* QsciLexerDASM16::wordCharacters() const
{
    return "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_#";
}

QColor QsciLexerDASM16::defaultColor(int style) const
{
    switch(style)
    {
        case Default:
            return QColor("#000000");
         case Comment:
            return QColor(0x00,0x7f,0x00);
         case Number:
            return QColor("#ff0000");
         case DoubleQuotedString:
         case SingleQuoteString:
            return QColor("#800000");
         case DCPU_Instruction:
            return QColor("#0000FF");
         case Register:
            return QColor("#8000FF");
         case DASM_Directive:
         case DASM_Directive_Operand:
            return QColor("#DF4000");
         case UnclosedString:
            return QColor("#E0C0E0");
    }
    return QsciLexer::defaultColor(style);
}

QFont QsciLexerDASM16::defaultFont(int style) const
{
    QFont f;

    switch(style)
    {
        case Comment:
            f.setItalic(true);
            break;
        case DCPU_Instruction:
        case Register:
            f = QsciLexer::defaultFont(style);
            break;
        case DoubleQuotedString:
        case SingleQuoteString:
        case UnclosedString:
            #if defined(Q_OS_WIN)
                f = QFont("Courier New",8);
            #else
                f = QFont("Bitstream Vera Sans Mono",8);
            #endif
            break;
        default:
            f = QsciLexer::defaultFont(style);
    }

    return f;
}

bool QsciLexerDASM16::defaultEolFill(int style) const
{
    if (style == UnclosedString)
        return true;

    return QsciLexer::defaultEolFill(style);
}

const char *QsciLexerDASM16::keywords(int set) const
{
    if(set == 1)
        return  "set add sub mul mli div dvi mod mdi and bor xor shr asr shl"
                "ifb ifc ife ife ifn ifg ifa ifl ifu adx sbx sti std jsr int"
                "iag ias rfi iaq hwn hwq hwi";
    else if(set == 2) //Registers..
        return  "a b c x y z i j pc sp ex ia";
    return 0;
}

QString QsciLexerDASM16::description(int style) const
{
    switch(style)
    {
        case Default:
            return tr("Default");
        case Comment:
            return tr("Comment");
        case Register:
            return tr("Registers");
        case DCPU_Instruction:
            return tr("CPU instructions");
        case Number:
            return tr("Numbers");
        case DASM_Directive:
            return tr("Directives");
        case DASM_Directive_Operand:
            return tr("Directive operands");
        case DoubleQuotedString:
            return tr("Double-quoted string");
        case SingleQuoteString:
            return tr("Single-quoted string");
    }

    return QString();
}

QColor QsciLexerDASM16::defaultPaper(int style) const
{
    if (style == UnclosedString)
        return QColor("#F7F5F0");

    return QsciLexer::defaultPaper(style);
}


void QsciLexerDASM16::setDollarsProp()
{
    emit propertyChanged("lexer.asm.allow.dollars",(dollars ? "1" : "0"));
}
