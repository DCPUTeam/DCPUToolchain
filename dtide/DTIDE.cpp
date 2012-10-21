#include "DTIDE.h"

DTIDE::DTIDE(Toolchain* t, QString fileName, QWidget* parent): QMainWindow(parent)
{
    menu = menuBar();

    tabs = new DTIDETabWidget(this);
    tabs->setMovable(true);
    setCentralWidget(tabs);
   
    setupMenuBar();
    setupActions();
    setupSignals();

    resize(QSize(640, 480));

    toolchain = t;
    addCodeTab(fileName);

    debuggingSession = 0;

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(cycleUpdate()));
    timer->start(1);
}

void DTIDE::cycleUpdate()
{
    if(debuggingSession != 0)
    {
        for(int i = 0; i < 100; i++) // 1ms = 1kHz, 100 * 1kHz = 100kHz
            this->toolchain->Cycle();

        while(debuggingSession->HasMessages())
        {
            DebuggingMessage m = debuggingSession->GetMessage();
            qDebug() << "Messages!";
        }
    }
}

void DTIDE::addCodeTab(const QString& fileName)
{
    QFont font;
    font.setFamily("Monospace");
    font.setFixedPitch(true);
    font.setPointSize(10);
    
    CodeEditor* editor = new CodeEditor(toolchain, fileName, this);
    connect(editor, SIGNAL(fileNameChanged(QString)), tabs, SLOT(updateTitle(QString)));
    editor->setFont(font);

    tabs->addTab(editor, fileName);
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
    connect(this, SIGNAL(fileSave()), tabs, SLOT(fileSave()));
}

void DTIDE::setupMenuBar()
{
    QMenu* file = new QMenu("&File", this);
    menu->addMenu(file);

    file->addAction("&New file", this, SLOT(newFile()), tr("Ctrl+N"));
    file->addAction("&Open file", this, SLOT(openFile()), tr("Ctrl+O"));
    file->addAction("&Save file", this, SLOT(saveFile()), tr("Ctrl+S"));

    QMenu* project = new QMenu("&Project", this);
    menu->addMenu(project);

    project->addAction("Compil&e", this, SLOT(compileProject()), tr("Ctrl+E"));
    project->addAction("Compile and &run", this, SLOT(compileAndRunProject()), tr("Ctrl+R"));
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
    emit fileSave();
}

QSize DTIDE::sizeHint()
{
    return QSize(640, 480);
}

void DTIDE::compileAndRunProject()
{
    debuggingSession = new DTIDEDebuggingSession();

    compileProject();
    for(int i = 0; i < tabs->count(); i++)
    {
    	CodeEditor* w = qobject_cast<CodeEditor*>(tabs->widget(i));
    	w->run(debuggingSession);
    }
}

void DTIDE::compileProject()
{
    for(int i = 0; i < tabs->count(); i++)
    {
    	CodeEditor* w = qobject_cast<CodeEditor*>(tabs->widget(i));
    	w->build();
    }
}



