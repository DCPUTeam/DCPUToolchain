#include "CodeEditor.h"

CodeEditor::CodeEditor(Toolchain* t, QString filename, QWidget* parent): QsciScintilla(parent)
{
    loading = false;
    highlighter = 0;
    
    connect(this, SIGNAL(textChanged()), this, SLOT(updateFileName()));
    connect(this, SIGNAL(linesChanged()), this, SLOT(updateLineNumberMarginWidth()));

    fileName = filename;
    // FIXME: Remove this; this just makes it so Ctrl-S doesn't ask for a filename.
    path = filename;
    toolchain = t;
    dirty = false;

    setHighlighter();
    setupEditor();
    setupMargins();
    setupSignals();

    readFile(filename);
}

void CodeEditor::readFile(QString path)
{
    QFile file(path);
    if(!file.open(QFile::ReadOnly))
        return;
    
    QTextStream stream(&file);
    loading = true;
    setText(stream.readAll());    
    loading = false;
}

void CodeEditor::setupEditor()
{

    setIndentationsUseTabs(false);
    setIndentationWidth(4);
    setWrapIndentMode(WrapIndentSame);
    setAutoIndent(true);
}

void CodeEditor::setupMargins()
{
    updateLineNumberMarginWidth();

    setMarginSensitivity(0, true);
    setMarginSensitivity(1, true);

    markerDefine(QsciScintilla::RightArrow, MARKER);
    setMarkerBackgroundColor(QColor("#ee1111"), MARKER);

    markerDefine(QsciScintilla::Background, LINE_BG);
    setMarkerBackgroundColor(QColor("#ffff00").lighter(170), LINE_BG);
}

void CodeEditor::setupSignals()
{
    connect(this, SIGNAL(marginClicked(int, int, Qt::KeyboardModifiers)), this, SLOT(handleMarginClick(int, int, Qt::KeyboardModifiers)));
}

void CodeEditor::handleMarginClick(int margin, int line, Qt::KeyboardModifiers kb)
{
    if(markersAtLine(line) != 0)
    {
        markerDelete(line, MARKER);
    }
    else
    {
        markerAdd(line, MARKER);
    }
}

void CodeEditor::highlightLine(int line)
{
    markerDeleteAll(LINE_BG);
    markerAdd(line - 1, LINE_BG);
}

void CodeEditor::stopHighlighting()
{
    markerDeleteAll(LINE_BG);
}

void CodeEditor::updateFileName()
{
    if(isModified() && !dirty && !loading)
    {
        fileName = fileName + "*";
        emit fileNameChanged(fileName);
        dirty = true;
    }
}

QString CodeEditor::getExtension()
{
    QStringList parts = fileName.split(".");
    QString last = parts[parts.count() - 1];

    return last;
}

void CodeEditor::setHighlighter()
{
    if (highlighter != 0)
        delete highlighter;

    std::string ext = getExtension().toStdString();
    lang = toolchain->GetLanguageByExtension(ext);

    if (lang != 0)
        highlighter = DTIDEHighlighting::getHighlighter(lang->GetCodeSyntax(), this);
    else
        highlighter = 0;
    
    setLexer(highlighter);
}

void CodeEditor::highlightCurrentLine()
{
    return;
}

void CodeEditor::saveFile()
{
    if (path.isEmpty())
    {
        path = QFileDialog::getSaveFileName(this);
    }

    QFileInfo f(path);
    fileName = f.fileName();
    emit fileNameChanged(fileName);

    dirty = false;
    QFile file(path);

    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&file);
        stream << text();
        stream << "\n";
    }


    setHighlighter();
}

void CodeEditor::updateLineNumberMarginWidth()
{
    int digits = 1;
    int max = qMax(1, lines());

    while(max >= 10)
    {
        max /= 10;
        digits++;
    }

    int width = 6 + fontMetrics().width(QLatin1Char('0')) * digits;

    setMarginWidth(0, width);
}

QString CodeEditor::getPath()
{
    if (path.isEmpty() || dirty)
    {
        saveFile();
    }

    return path;
}

QString CodeEditor::getFileName()
{
    return fileName;
}

void CodeEditor::ResetBuild()
{
    // Create a new Build API instance.  The "build API" class should be used as
    // a temporary object while the build is occurred and the subsequent debug
    // execution, but discarded when a new build starts.
    this->buildAPI = DTIDEBuildAPI();
}

bool CodeEditor::build()
{
    QFileInfo f(getPath());
    lang->Build(path.toStdString(), f.absolutePath().toStdString(), buildAPI);

    return true;
}

void CodeEditor::run(DebuggingSession* s)
{
    toolchain->Start(buildAPI, s);
}

QList<Breakpoint> CodeEditor::getBreakpoints()
{
    QList<Breakpoint> res;
    Breakpoint b;

    int prev = 0;
    int line = -1;

    do
    {
        line = markerFindNext(prev, (1 << MARKER));
        if(line != -1)
        {
            line += 1;
            prev = line;

            b.Line = line;
            b.File = fileName.toLocal8Bit().constData();

            res.push_back(b);
        }
    } while(line != -1);

    return res;
}
