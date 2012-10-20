#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>
#include <QPaintEvent>
#include <QObject>
#include <QTextBlock>
#include <QPainter>
#include <QTextStream>
#include <QDebug>
#include <QSyntaxHighlighter>
#include <QFileDialog>
#include <QFileInfo>

#include "DTIDEHighlighting.h"
#include "DTIDEBuildAPI.h"
#include "Backends.h"

class CodeEditor: public QPlainTextEdit
{
    Q_OBJECT

public:
    CodeEditor(Toolchain* t, QString file, QWidget* parent = 0);

    void lineNumberAreaPaintEvent(QPaintEvent* event);
    int lineNumberAreaWidth();
    void saveFile();

    QString getPath();
    bool build();
    void run();

signals:
    void fileNameChanged(QString);

protected:
    void resizeEvent(QResizeEvent* event);
    void keyPressEvent(QKeyEvent* event);
    QString getExtension(); 
    void setHighlighter();

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect&, int);

private:
    QWidget *lineNumberArea;
    QSyntaxHighlighter* highlighter;
    Toolchain* toolchain;
    Language* lang;
    QString fileName;
    QString path;

    DTIDEBuildAPI buildAPI;

    bool dirty;
};

class LineNumberArea: public QWidget
{
public:
    LineNumberArea(CodeEditor* editor): QWidget(editor)
    {
	codeEditor = editor;
    }

    QSize sizeHint() const
    {
	return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent* event)
    {
	codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    CodeEditor* codeEditor;
};

#endif
