#include "dtidesplash.h"

DTIDESplash::DTIDESplash(std::list<Toolchain*> toolchains, QWidget* parent): QDialog(parent)
{
    setupUi(this);
    connect(btn_new, SIGNAL(clicked()), this, SLOT(setAndAccept()));
    connect(btn_cancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(btn_open, SIGNAL(clicked()), this, SLOT(openAndAccept()));
}

void DTIDESplash::setAndAccept()
{
    fileName = lineEdit->text();
    QString sType = combo_project->currentText();
    
    accept();
}

void DTIDESplash::openAndAccept()
{
    return;
}

