#include "DTIDERegisters.h"

DTIDERegisters::DTIDERegisters(QWidget* parent): QWidget(parent)
{
    setupUi(this);
    connect(btn_step, SIGNAL(clicked()), this, SIGNAL(step()));
    connect(btn_run, SIGNAL(clicked()), this, SIGNAL(start()));
    connect(btn_stop, SIGNAL(clicked()), this, SIGNAL(pause()));
}

QSize DTIDERegisters::sizeHint()
{
    return QSize(400, 400);
}

void DTIDERegisters::setRegisters(StatusMessage m)
{
    QStringList registers;
    for(int i = 0; i < 8; i++)
    {
        QString r = QString("0x%1").arg(m.registers[i], 4, 16, QChar('0'));
        registers.append(r);
    }
    
    reg_A->setText(registers[0]);
}
