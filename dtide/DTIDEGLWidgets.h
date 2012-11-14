#ifndef DTIDEGLWIDGETS_H
#define DTIDEGLWIDGETS_H

#include <QGLWidget>
class DTIDEGLWidget;
#include "Backends.h"
#include "DTIDEGLWidget.h"

class DTIDEGLWidgets: public QObject
{
    Q_OBJECT

public:
    DTIDEGLWidgets();

    DTIDEGLWidget* requestWidget(Toolchain* toolchain, QString title, int width, int height, void* ud);
    void killWidgets();

signals:
    void spawnGLWidget(QGLWidget*, QString, int, int);
    void killDockWidget(QGLWidget*);

private:
    QList<QGLWidget*> activeWidgets;
};

#endif
