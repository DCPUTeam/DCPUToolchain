#ifndef DTIDESPLASH_H
#define DTDIESPLASH_H

#include <QDebug>

#include "backends.h"
#include "ui_splash.h"

class DTIDESplash: public QDialog, private Ui::splash
{
    Q_OBJECT

public:
    DTIDESplash(QWidget* parent = 0);
    ProjectProperties getProperties();

private slots:
    void setAndAccept();
    void openAndAccept();

private:
    QString fileName;
    int type;
};

#endif
