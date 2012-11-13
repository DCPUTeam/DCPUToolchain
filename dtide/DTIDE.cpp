#include <cassert>
#include <QDebug>
#include "DTIDE.h"

#include <iostream>

DTIDE::DTIDE(Project* p, QWidget* parent): QMainWindow(parent)
{
    debuggingWindow = 0;
    menu = menuBar();

    tabs = new DTIDETabWidget(this);
    tabs->setMovable(true);
    setCentralWidget(tabs);

    debuggingSession = 0;
    glWidgets = new DTIDEGLWidgets();
    project = p;
    toolchain = project->getToolchain();
    toolchain->SetWidgetFactory(glWidgets);

    setWindowTitle("dtide - " + project->getTitle());

    setupMenuBar();
    setupActions();
    setupSignals();
    setupDockWidgets();

    resize(QSize(640, 580));

    QDir::setCurrent(project->getRootPath());
    
    QList<QString> files = project->getFileTabs();
    for(int i = 0; i < files.size(); i++) 
    {
        addCodeTab(files[i]);
    }
  
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(cycleUpdate()));
    timer->start(1);
    
    // start the frequency timer, that checks and prints
    // the current event loop frequency
    // FIXME: this is just for debugging! please leave it here for now - r4d2
    vm_timing_start_timer(&freq_timer);
    
    // init timers
    vm_timing_start_timer(&cycle_timer);
    vm_timing_start_timer(&db_update_timer);
}

void DTIDE::cycleUpdate()
{
    int db_update_hz = 15;
    int cycle_hz = 120;
    
    //if(!this->toolchain->IsPaused())
    //    runCycles(1667); // 1ms = 1kHz, 100 * 1kHz = 100kHz
    cycle_count++;
    
    
    if(!this->toolchain->IsPaused())
    {
        // run at cycle_hz Hz, and emulate as many cycles as necessary
        // to have total speed 100 kHz
        if (vm_timing_has_reached_mics(&cycle_timer, 1000000/cycle_hz))
        {
            // run cycles
            runCycles(100000/cycle_hz); 
            vm_timing_reset_timer(&cycle_timer);
        }
        
        // update the debugger UI at 15 Hz (more than enough)
        if (vm_timing_has_reached_mics(&db_update_timer, 1000000/db_update_hz))
        {
            // update debugger interface
            updateDebuggerUI();
            vm_timing_reset_timer(&db_update_timer);
        }
    }
    
    // FIXME: only for debugging, please leave it here for now - r4d2
    
    /*
    if (vm_timing_has_reached_mics(&freq_timer, 1000000))
    {
        std::cout << "current speed: " << cycle_count << " Hz" << std::endl;
        vm_timing_reset_timer(&freq_timer);
        cycle_count = 0;
    }
    */
}


void DTIDE::highlightCurrentLine()
{
    Line line = debuggingSession->GetLine();
    for (int i = 0; i < tabs->count(); i++)
    {
        CodeEditor* w = qobject_cast<CodeEditor*>(tabs->widget(i));
        if(w->getFileName() == QString::fromLocal8Bit(line.Path))
            w->highlightLine(line.LineNumber);
    }
}

void DTIDE::updateDebuggerUI()
{
    if(debuggingSession == NULL)
        return;

    // highlight current line
    highlightCurrentLine();

    // update the memory view (hex view)
    uint16_t* store = debuggingSession->getMemory();
    QByteArray* ram = new QByteArray();
    for(int i = 0; i < 0x10000; i++)
    {
        ram->push_back(store[i] >> 8);
        ram->push_back(store[i] & 0xff);
    }
    debuggingWindow->setMemoryData(ram);
    
    // update status and ??LineHitType??
    this->toolchain->SendStatus();
    
    while (debuggingSession->HasMessages())
    {
        DebuggingMessage m = debuggingSession->GetMessage();
        switch (m.type)
        {
            case StatusType:
            {
                StatusMessage status = (StatusMessage&) m.value;
                debuggingWindow->setRegisters(status);
                break;
            }
            case LineHitType:
            {
                LineHitMessage hit = (LineHitMessage&) m.value;
                Line line = hit.line;
               
                break;
            }
            default:
                // Not implemented.
                break;
        }
    }
}

void DTIDE::runCycles(int count)
{
    if (debuggingSession != 0)
    {
        if (count == 1)
        {
            this->toolchain->Step();
        }
        else
        {
            for (int i = 0; i < count; i++)
            {
                this->toolchain->Cycle();
                //highlightCurrentLine();
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
    connect(debuggingWindow, SIGNAL(step()), this, SLOT(step()));
    connect(debuggingWindow, SIGNAL(resume()), this, SLOT(resume()));
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

    QDockWidget* dirViewDockWidget = new QDockWidget(tr("File system"), this);
    dirViewDockWidget->setWidget(new DTIDEDirView(project->getRootPath()));
    addDockWidget(Qt::LeftDockWidgetArea, dirViewDockWidget); 
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

    for (int i = 0; i < tabs->count(); i++)
    {
        CodeEditor* w = qobject_cast<CodeEditor*>(tabs->widget(i));
        w->stopHighlighting();
    }

    toolchain->Stop(debuggingSession);
}

void DTIDE::pause()
{
    toolchain->Pause(debuggingSession);
}

void DTIDE::resume()
{
    toolchain->Resume(debuggingSession);
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

    QList<QString> compilableFiles = project->getCompilableFiles();
    for (int i = 0; i < tabs->count(); i++)
    {
        CodeEditor* w = qobject_cast<CodeEditor*>(tabs->widget(i));
        if(compilableFiles.contains(w->getFileName()))
        {
            qDebug() << "running " << w->getFileName();
            QList<Breakpoint> breakpoints(w->getBreakpoints());
            w->run(debuggingSession);
    
            if(!breakpoints.empty())
            {
                for(int i = 0; i < breakpoints.size(); i++)
                {
                    toolchain->AddBreakpoint(debuggingSession, breakpoints[i]);
                }
            }
        } 
    }
}

void DTIDE::compileProject()
{
    QList<QString> compilableFiles = project->getCompilableFiles();
    for (int i = 0; i < tabs->count(); i++)
    {
        CodeEditor* w = qobject_cast<CodeEditor*>(tabs->widget(i));
        if(compilableFiles.contains(w->getFileName()))
        {
            qDebug() << "compiling";
            w->ResetBuild();
            w->build();
        }
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
