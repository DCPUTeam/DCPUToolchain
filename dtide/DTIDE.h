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

#include "Backends.h"
#include "CodeEditor.h"
#include "DTIDETabWidget.h"

class DTIDE: public QMainWindow
{
    Q_OBJECT

public:
    DTIDE(Toolchain* t, QString filename, QWidget* parent = 0);

    QSize sizeHint();

protected:
    void addCodeTab(const QString& fileName);

signals:
    void fileSave();

private slots:
   void newFile();
   void openFile(); 
   void saveFile();

   void cycleUpdate();

   void compileProject();
   void compileAndRunProject();

private:
    DTIDETabWidget* tabs;
    QMenuBar* menu;
    QAction* nextTab;
    Toolchain* toolchain;
    QTimer* timer;

    void setupMenuBar();
    void setupActions();
    void setupSignals();
};

#endif
