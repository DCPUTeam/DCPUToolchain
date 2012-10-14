#include "codeeditor.h"

CodeEditor::CodeEditor(Toolchain* t, QString filename, QWidget* parent): QPlainTextEdit(parent)
{
    lineNumberArea = new LineNumberArea(this);
    
    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect, int)), this, SLOT(updateLineNumberArea(QRect, int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    emit blockCountChanged(0);
    highlightCurrentLine();

    fileName = filename;
    toolchain = t;
    dirty = false;

    setHighlighter();
}

QString CodeEditor::getExtension()
{
    QStringList parts = fileName.split(".");
    QString last = parts[parts.count() - 1];

    return last;
}

void CodeEditor::setHighlighter()
{
    if(highlighter != NULL)
        delete highlighter;

    std::string ext = getExtension().toStdString();
    lang = toolchain->GetLanguageByExtension(ext);
    
    if(lang != NULL)
        highlighter = DTIDEHighlighting::getHighlighter(lang->GetCodeSyntax(), document());
    else
        highlighter = NULL;

}


void CodeEditor::updateLineNumberAreaWidth(int newBlockCount)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

int CodeEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());

    while(max >= 10)
    {
        max /= 10;
        digits++;
    }

    return 6 + fontMetrics().width(QLatin1Char('1')) * digits;
}



void CodeEditor::updateLineNumberArea(const QRect& rect, int dy)
{
    if(dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if(rect.contains(viewport()->rect()))
        emit blockCountChanged(0);
}

void CodeEditor::resizeEvent(QResizeEvent* e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::highlightCurrentLine()
{
    return;
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent* event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while(block.isValid() && top <= event->rect().bottom())
    {
        if(block.isVisible() && bottom >= event->rect().top())
        {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);

            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(), Qt::AlignCenter, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
   }
}

void CodeEditor::keyPressEvent(QKeyEvent* event)
{
    QKeyEvent* space = new QKeyEvent(
            event->type(),
            Qt::Key_Space,
            event->modifiers(),
            QString(" "));

    if(event->key() == Qt::Key_Tab)
    {
        for(int i = 0; i < 4; i++)
            QPlainTextEdit::keyPressEvent(space);
    }
    else if(event->key() == Qt::Key_Return)
    {
        QTextCursor c = textCursor();
        c.movePosition(QTextCursor::StartOfBlock);
        c.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        
        QString previousLine = c.selectedText();
        int count = 0;
        int idx = 0;
        while(previousLine[idx++] == ' ') count++;
        
        QPlainTextEdit::keyPressEvent(event);
        for(int i = 0; i < count; i++) 
            QPlainTextEdit::keyPressEvent(space);
    }
    else
    {
       QPlainTextEdit::keyPressEvent(event);
    }

    if(!dirty && !event->text().isEmpty())
    {
        dirty = true;
        fileName += "*";
        emit fileNameChanged(fileName);
    }
}

void CodeEditor::saveFile(QString path, QString name)
{
    fileName = name;
    dirty = false;

    QFile file(path);
    if(file.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&file);
        stream << toPlainText();
        stream << "\n";
    }

    setHighlighter();
}
