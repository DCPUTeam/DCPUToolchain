#include <cassert>
#include "DTIDE.h"

DTIDE::DTIDE(Toolchain* t, QString fileName, QWidget* parent): QMainWindow(parent)
{
    debuggingWindow = 0;
    menu = menuBar();

    tabs = new DTIDETabWidget(this);
    tabs->setMovable(true);
    setCentralWidget(tabs);

    debuggingSession = 0;
    glWidgets = new DTIDEGLWidgets();
    toolchain = t;
    toolchain->SetWidgetFactory(glWidgets);

    setupMenuBar();
    setupActions();
    setupSignals();
    setupDockWidgets();

    resize(QSize(640, 580));

    addCodeTab(fileName);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(cycleUpdate()));
    timer->start(1);
}

void DTIDE::cycleUpdate()
{
    runCycles(100); // 1ms = 1kHz, 100 * 1kHz = 100kHz
}

void DTIDE::runCycles(int count)
{
    if (debuggingSession != 0)
    {
        if (count == 1)
            this->toolchain->Step();
        else
            for (int i = 0; i < count; i++)
                this->toolchain->Cycle();

        this->toolchain->SendStatus();

        while (debuggingSession->HasMessages())
        {
            DebuggingMessage m = debuggingSession->GetMessage();
            switch (m.type)
            {
                case StatusType:
                {
                    StatusMessage status = (StatusMessage&) m.value;
                    emit setRegisters(status);
                    break;
                }
                case MemoryDumpType:
                {
                    MemoryDumpMessage memory = (MemoryDumpMessage&) m.value;
                    uint16_t* store = memory.data;
                    QByteArray* ram = new QByteArray();
                    for(int i = 0; i < 0x10000; i++)
                    {
                        ram->push_back(store[i] >> 8);
                        ram->push_back(store[i] & 0xff);
                    }

                    debuggingWindow->setMemoryData(ram);
                    break;
                }
                case MemoryType:
                {
                    MemoryMessage memory = (MemoryMessage&) m.value;
                    debuggingWindow->setMemoryAt(memory.pos, memory.value >> 8, memory.value & 0xff);
                    break;
                }
                default:
                    // Not implemented.
                    break;
            }
        }
    }
}


void DTIDE::addCodeTab(const QString& fileName)
{

    CodeEditor* editor = new CodeEditor(toolchain, fileName, this);
    connect(editor, SIGNAL(fileNameChanged(QString)), tabs, SLOT(updateTitle(QString)));

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
    connect(glWidgets, SIGNAL(spawnGLWidget(QGLWidget*, QString, int, int)), this, SLOT(addGLWidget(QGLWidget*, QString, int, int)));
    connect(glWidgets, SIGNAL(killDockWidget(QGLWidget*)), this, SLOT(killDockWidget(QGLWidget*)));
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

void DTIDE::showDebuggerWindow()
{
    debuggingWindow = new DTIDEDebuggingWindow(this);
    connect(this, SIGNAL(setRegisters(StatusMessage)), debuggingWindow, SLOT(setRegisters(StatusMessage)));
    connect(debuggingWindow, SIGNAL(step()), this, SLOT(step()));
    connect(debuggingWindow, SIGNAL(start()), this, SLOT(compileAndRunProject()));
    connect(debuggingWindow, SIGNAL(pause()), this, SLOT(pause()));
    connect(debuggingWindow, SIGNAL(stop()), this, SLOT(stop()));

    debuggingWindow->show();
}

void DTIDE::setupDockWidgets()
{
/*    QDockWidget* registersDockWidget = new QDockWidget(tr("Registers"), this);
    registersDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea |
                                         Qt::RightDockWidgetArea);

    registersDockWidget->setWidget(registers);
    registersDockWidget->setMinimumWidth(100);
    addDockWidget(Qt::RightDockWidgetArea, registersDockWidget);*/
}

void DTIDE::addGLWidget(QGLWidget* w, QString title, int width, int height)
{
    QDockWidget* glDockWidget = new QDockWidget(title, this);
    glDockWidget->setWidget(w);
    glDockWidget->setMinimumWidth(width);
    glDockWidget->setMinimumHeight(height);
    glDockWidget->setFloating(true);
    addDockWidget(Qt::LeftDockWidgetArea, glDockWidget);

    dockWidgets.append(glDockWidget);
}

void DTIDE::step()
{
    runCycles(1);
}

void DTIDE::stop()
{
    if(debuggingWindow)
    {
        debuggingWindow = 0;
    }
    toolchain->Stop(debuggingSession);
}

void DTIDE::pause()
{
    toolchain->Pause(debuggingSession);
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
    if(debuggingWindow)
        debuggingWindow->close();
    stop();
    debuggingSession = new DTIDEDebuggingSession();
    compileProject();

    showDebuggerWindow();

    for (int i = 0; i < tabs->count(); i++)
    {
        CodeEditor* w = qobject_cast<CodeEditor*>(tabs->widget(i));
        w->run(debuggingSession);
    }
}

void DTIDE::compileProject()
{
    for (int i = 0; i < tabs->count(); i++)
    {
        CodeEditor* w = qobject_cast<CodeEditor*>(tabs->widget(i));
        w->build();
    }
}

void DTIDE::closeEvent(QCloseEvent* event)
{
    // clean up
}

void DTIDE::killDockWidget(QGLWidget* w)
{
    for (int i = 0; i < dockWidgets.size(); i++)
    {
        QDockWidget* dW = dockWidgets[i];
        if (dW->widget() == w)
        {
            removeDockWidget(dW);
            dockWidgets.removeAt(i);
            return;
        }
    }
}
