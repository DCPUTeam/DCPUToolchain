#include "CodeEditor.h"

CodeEditor::CodeEditor(Toolchain* t, QString filename, QWidget* parent): QsciScintilla(parent)
{
    highlighter = 0;
    
    connect(this, SIGNAL(textChanged()), this, SLOT(updateFileName()));
    connect(this, SIGNAL(linesChanged()), this, SLOT(updateLineNumberMarginWidth()));

    fileName = filename;
    // FIXME: Remove this; this just makes it so Ctrl-S doesn't ask for a filename.
    path = filename;
    toolchain = t;
    dirty = false;

    setHighlighter();
    setMarginType(1, NumberMargin);
    setIndentationsUseTabs(false);
    setIndentationWidth(4);
    setWrapIndentMode(WrapIndentSame);
    setAutoIndent(true);
}

void CodeEditor::updateFileName()
{
    if(isModified() && !dirty)
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

    int width = fontMetrics().width(QLatin1Char('0')) * digits + 6;

    setMarginWidth(1, width);
}

QString CodeEditor::getPath()
{
    if (path.isEmpty())
    {
        saveFile();
    }

    return path;
}

bool CodeEditor::build()
{
    QFileInfo f(getPath());
    std::string absolutePath = f.absolutePath().toStdString();

    lang->Build(path.toStdString(), absolutePath, buildAPI);

    return true;
}

void CodeEditor::run(DebuggingSession* s)
{
    toolchain->Start(buildAPI.output, s);
}

