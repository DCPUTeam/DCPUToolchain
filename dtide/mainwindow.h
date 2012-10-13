#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QKeySequence>
#include <QDebug>

#include "codeeditor.h"

class DTIDE: public QMainWindow
{
    Q_OBJECT

public:
    DTIDE(QWidget* parent = 0);

protected:
    void addCodeTab(const QString& title);

public slots:
   void goToNextTab();

private slots:
   void newFile();
   void openFile(); 

private:
    QTabWidget* tabs;
    QMenuBar* menu;
    QAction* nextTab;

    void setupMenuBar();
};

#endif
