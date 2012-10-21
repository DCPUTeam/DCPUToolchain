#ifndef DTIDEREGISTERS_H
#define DTIDEREGISTERS_H

#include <QWidget>
#include <QDebug>

#include "Backends.h"
#include "ui_registers.h"

class DTIDERegisters: public QWidget, private Ui::registers
{
    Q_OBJECT

public:
    DTIDERegisters(QWidget* parent = 0);
    QSize sizeHint();

public slots:
    void setRegisters(StatusMessage);

};

#endif
