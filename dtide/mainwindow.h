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

#include "codeeditor.h"
#include "dtidetabwidget.h"

class DTIDE: public QMainWindow
{
    Q_OBJECT

public:
    DTIDE(QWidget* parent = 0);

protected:
    void addCodeTab(const QString& title);

signals:
    void fileSave(QString);

private slots:
   void newFile();
   void openFile(); 
   void saveFile();

private:
    DTIDETabWidget* tabs;
    QMenuBar* menu;
    QAction* nextTab;

    void setupMenuBar();
    void setupActions();
    void setupSignals();
};

#endif
