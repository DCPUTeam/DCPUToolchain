#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QThread>
#include "ui_dtdb.h"
#include "dtdbrunner.h"

class MainWindow: public QMainWindow, private Ui::DTDB
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = 0);

private slots:
    void updateVM(QList<QString>);

public slots:
    void setStatusBar(QString s);
    void stop();
    void load();

signals:
    void runCommand(QString);

private:
    DTDBRunner* runner;
};

#endif
