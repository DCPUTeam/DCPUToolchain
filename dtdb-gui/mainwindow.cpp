#include <QtGui>
#include <QtDebug>
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
{
    setupUi(this);
    runner = new DTDBRunner();
    connect(this, SIGNAL(runCommand(QString)), runner, SLOT(runCommand(QString)));

    connect(btn_load, SIGNAL(clicked()), this, SLOT(load()));
    connect(btn_run, SIGNAL(clicked()), runner, SLOT(run()));
    connect(btn_stop, SIGNAL(clicked()), runner, SLOT(stop()));

    connect(runner, SIGNAL(vmUpdated(QList<QString>)), this, SLOT(updateVM(QList<QString>)));

    emit runCommand("attach hardware lem1802");
    emit runCommand("attach hardware clock");
}

void MainWindow::load()
{
    emit runCommand("load clocktest.o");
}

void MainWindow::setStatusBar(QString s)
{
    statusbar->showMessage(s);
}

void MainWindow::updateVM(QList<QString> registers)
{
    reg_A->setText(registers[1]);
    reg_B->setText(registers[2]);
    reg_C->setText(registers[3]);
    reg_X->setText(registers[4]);
    reg_Y->setText(registers[5]);
    reg_Z->setText(registers[6]);
    reg_I->setText(registers[7]);
    reg_Z->setText(registers[8]);
    reg_PC->setText(registers[9]);
    reg_SP->setText(registers[10]);
    reg_IA->setText(registers[11]);
    reg_EX->setText(registers[12]);
}

void MainWindow::stop()
{
}
