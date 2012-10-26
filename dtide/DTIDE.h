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

#include "Backends.h"
#include "CodeEditor.h"
#include "DTIDETabWidget.h"
#include "DTIDERegisters.h"
#include "DTIDEDebuggingSession.h"
#include "DTIDEGLWidgets.h"

class DTIDE: public QMainWindow
{
    Q_OBJECT

public:
    DTIDE(Toolchain* t, QString filename, QWidget* parent = 0);

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

    void compileProject();
    void compileAndRunProject();

    void addGLWidget(QGLWidget*, QString, int, int);
    void killDockWidget(QGLWidget*);

private:
    DTIDETabWidget* tabs;
    DTIDERegisters* registers;
    DTIDEDebuggingSession* debuggingSession;
    DTIDEGLWidgets* glWidgets;
    QMenuBar* menu;
    QAction* nextTab;
    Toolchain* toolchain;
    QTimer* timer;
    QList<QDockWidget*> dockWidgets;

    void setupMenuBar();
    void setupActions();
    void setupSignals();
    void setupDockWidgets();
};

#endif
