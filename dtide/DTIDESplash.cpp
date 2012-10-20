#include "DTIDESplash.h"

DTIDESplash::DTIDESplash(std::list<Toolchain*> t, QWidget* parent): QDialog(parent)
{
    setupUi(this);
    
    toolchains = t;

    setupComboBox();

    connect(btn_new, SIGNAL(clicked()), this, SLOT(setAndAccept()));
    connect(btn_cancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(btn_open, SIGNAL(clicked()), this, SLOT(openAndAccept()));
}

void DTIDESplash::setupComboBox()
{
    std::list<Toolchain*>::iterator it;
    for(it = toolchains.begin(); it != toolchains.end(); it++)
    {
	QString label = QString::fromStdString(dynamic_cast<Toolchain*>(*it)->GetName());
	combo_project->insertItem(0, label);
    }
}

void DTIDESplash::setAndAccept()
{
    fileName = lineEdit->text();
    QString sType = combo_project->currentText();
   
    std::list<Toolchain*>::iterator it;
    for(it = toolchains.begin(); it != toolchains.end(); it++)
    {
	Toolchain* curr = dynamic_cast<Toolchain*>(*it);
	QString label = QString::fromStdString(curr->GetName());
	if(label == sType)
	    toolchain = curr;
    }

    accept();
}

void DTIDESplash::openAndAccept()
{
    return;
}

