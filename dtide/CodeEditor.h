#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QObject>
#include <QTextStream>
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <qsciscintilla.h>
#include <qscilexer.h>

#include "DTIDEHighlighting.h"
#include "DTIDEBuildAPI.h"
#include "Backends.h"

#define MARKER 2
#define LINE_BG 3

class CodeEditor: public QsciScintilla 
{
    Q_OBJECT

public:
    CodeEditor(Toolchain* t, QString file, QWidget* parent = 0);

    void lineNumberAreaPaintEvent(QPaintEvent* event);
    void saveFile();

    QString getPath();
    QString getFileName();
    void ResetBuild();
    bool build();
    QList<Breakpoint> getBreakpoints();
    void run(DebuggingSession* s);

    void highlightLine(int line);
    void stopHighlighting();

signals:
    void fileNameChanged(QString);

protected:
    QString getExtension();
    void setHighlighter();

private slots:
    void highlightCurrentLine();
    void updateFileName();
    void updateLineNumberMarginWidth();
    void handleMarginClick(int, int, Qt::KeyboardModifiers);

private:
    QsciLexer* highlighter;
    Toolchain* toolchain;
    Language* lang;
    QString fileName;
    QString path;

    DTIDEBuildAPI buildAPI;

    int marginPadding;
    bool dirty;
    bool loading;

    void setupEditor();
    void setupMargins();
    void setupSignals();
    void readFile(QString path);
};

#endif
