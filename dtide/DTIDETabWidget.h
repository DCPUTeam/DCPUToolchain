#ifndef DTIDETABWIDGET_H
#define DTIDETABWIDGET_H

#include <QTabWidget>
#include <QFileInfo>

#include "CodeEditor.h"

class DTIDETabWidget: public QTabWidget
{
    Q_OBJECT

public:
    DTIDETabWidget(QWidget* parent = 0);

    QSize sizeHint();

public slots:
    void goToNextTab();
    void updateTitle(QString);
    void fileSave();
};

#endif
