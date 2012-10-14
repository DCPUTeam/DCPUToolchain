#include "mainwindow.h"

DTIDE::DTIDE(QWidget* parent): QMainWindow(parent)
{
    menu = menuBar();

    tabs = new DTIDETabWidget(this);
    tabs->setMovable(true);
    setCentralWidget(tabs);
   
    setupMenuBar();
    setupActions();
    setupSignals();

    resize(QSize(640, 480));

//    addCodeTab(p);
}

void DTIDE::addCodeTab()
{
    QFont font;
    font.setFamily("Monospace");
    font.setFixedPitch(true);
    font.setPointSize(10);
/*
    CodeEditor* editor = new CodeEditor(p, this);
    connect(editor, SIGNAL(fileNameChanged(QString)), tabs, SLOT(updateTitle(QString)));
    editor->setFont(font);

    tabs->addTab(editor, p.fileName);*/
}

void DTIDE::setupActions()
{
    nextTab = new QAction(tr("Next tab"), this);
    nextTab->setShortcut(QKeySequence::NextChild);
    connect(nextTab, SIGNAL(triggered()), tabs, SLOT(goToNextTab()));
    addAction(nextTab);
}

void DTIDE::setupSignals()
{
    connect(this, SIGNAL(fileSave(QString)), tabs, SLOT(fileSave(QString)));
}

void DTIDE::setupMenuBar()
{
    QMenu* file = new QMenu("&File", this);
    menu->addMenu(file);

    file->addAction("&New file", this, SLOT(newFile()), tr("Ctrl+N"));
    file->addAction("&Open file", this, SLOT(openFile()), tr("Ctrl+O"));
    file->addAction("&Save file", this, SLOT(saveFile()), tr("Ctrl+S"));
}

void DTIDE::newFile()
{
//    addCodeTab(DTIDEBackends::getUntitledProperties(type));
}

void DTIDE::openFile()
{
}

void DTIDE::saveFile()
{
    QString fileName = QFileDialog::getSaveFileName(this);
    emit fileSave(fileName);
}

QSize DTIDE::sizeHint()
{
    return QSize(640, 480);
}

