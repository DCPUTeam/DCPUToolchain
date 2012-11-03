#ifndef DTIDEREGISTERS_H
#define DTIDEREGISTERS_H

#include <QDialog>
#include <QWidget>
#include <QDebug>
#include <QCloseEvent>
#include <QBuffer>

#include "Backends.h"
#include "ui_registers.h"

class DTIDEDebuggingWindow: public QDialog, private Ui::registers
{
    Q_OBJECT

public:
    DTIDEDebuggingWindow(QWidget* parent = 0);
    QSize sizeHint();

public slots:
    void setRegisters(StatusMessage);
    void setMemoryData(QByteArray*);

signals:
    void start();
    void pause();
    void step();
    void stop();

protected:
    virtual void closeEvent(QCloseEvent* e);

private:
    QByteArray* data;
    QBuffer* buffer;

    bool buffer_initialized;
    void initializeBuffer();
};

#endif
