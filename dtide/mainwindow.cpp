#include "mainwindow.h"

DTIDE::DTIDE(QWidget* parent): QMainWindow(parent)
{
    menu = menuBar();

    tabs = new QTabWidget(this);
    tabs->setMovable(true);
    setCentralWidget(tabs);
   
    setupMenuBar();
    
    addCodeTab("untitled.dasm");

    nextTab = new QAction(tr("Next tab"), this);
    nextTab->setShortcut(tr("Ctrl+Tab"));
    connect(nextTab, SIGNAL(triggered()), this, SLOT(goToNextTab()));
    addAction(nextTab);
}

void DTIDE::addCodeTab(const QString& title)
{
    QFont font;
    font.setFamily("Monospace");
    font.setFixedPitch(true);
    font.setPointSize(10);

    CodeEditor* editor = new CodeEditor(this);
    editor->setFont(font);

    tabs->addTab(editor, title);
}

void DTIDE::setupMenuBar()
{
    QMenu* file = new QMenu("&File", this);
    menu->addMenu(file);

    file->addAction("&New file", this, SLOT(newFile()), tr("Ctrl+N"));
    file->addAction("&Open file", this, SLOT(openFile()), tr("Ctrl+O"));

}

void DTIDE::newFile()
{
    addCodeTab("untitled.dasm");
}

void DTIDE::openFile()
{
    addCodeTab("opened_file.dasm");
}

void DTIDE::goToNextTab()
{
    int cur = tabs->currentIndex() + 1;

    if(cur <= tabs->count() - 1)
        tabs->setCurrentIndex(cur);
    else
        tabs->setCurrentIndex(0);
}
