#ifndef DTIDETABWIDGET_H
#define DTIDETABWIDGET_H

#include <QTabWidget>
#include <QFileInfo>

#include "codeeditor.h"

class DTIDETabWidget: public QTabWidget
{
    Q_OBJECT

public:
    DTIDETabWidget(QWidget* parent = 0);

public slots:
    void goToNextTab();
    void updateTitle(QString);
    void fileSave(QString);
};

#endif
