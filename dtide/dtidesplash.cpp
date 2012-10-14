#include "dtidesplash.h"

DTIDESplash::DTIDESplash(QWidget* parent): QDialog(parent)
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
    
    if(sType == "DCPU Assembly")
        type = DTIDEBackends::DCPU_ASSEMBLY;
    else if(sType == "DCPU Toolchain C")
        type = DTIDEBackends::DCPU_TOOLCHAIN_C;
    else if(sType == "DCPU Brainfuck")
        type = DTIDEBackends::DCPU_BRAINFUCK;

    accept();
}

void DTIDESplash::openAndAccept()
{
    return;
}

ProjectProperties DTIDESplash::getProperties()
{
    ProjectProperties p;
    p.type = type;
    p.fileName = fileName;
    
    return p;
} 
