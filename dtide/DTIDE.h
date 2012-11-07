#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QKeySequence>
#include <QDebug>
#include <QFileDialog>
#include <QTimer>
#include <QDockWidget>

#include "Project.h"
#include "Backends.h"
#include "CodeEditor.h"
#include "DTIDETabWidget.h"
#include "DTIDEDebuggingWindow.h"
#include "DTIDEDebuggingSession.h"
#include "DTIDEGLWidgets.h"
#include "DTIDEDirView.h"

class DTIDE: public QMainWindow
{
    Q_OBJECT

public:
    DTIDE(Project* p, QWidget* parent = 0);

    QSize sizeHint();

protected:
    void addCodeTab(const QString& fileName);
    void runCycles(int count);
    void closeEvent(QCloseEvent* event);

signals:
    void fileSave();
    void setRegisters(StatusMessage);

private slots:
    void newFile();
    void openFile();
    void saveFile();

    void cycleUpdate();
    void step();
    void stop();
    void pause();
    void resume();

    void compileProject();
    void compileAndRunProject();

    void addGLWidget(QGLWidget*, QString, int, int);
    void killDockWidget(QGLWidget*);

private:
    DTIDETabWidget* tabs;
    DTIDEDebuggingWindow* debuggingWindow;
    DTIDEDebuggingSession* debuggingSession;
    DTIDEGLWidgets* glWidgets;
    QMenuBar* menu;
    QAction* nextTab;
    QTimer* timer;
    QList<QDockWidget*> dockWidgets;
    Project* project;
    Toolchain* toolchain;

    void setupMenuBar();
    void setupActions();
    void setupSignals();
    void setupDockWidgets();

    void showDebuggerWindow();
};

#endif
