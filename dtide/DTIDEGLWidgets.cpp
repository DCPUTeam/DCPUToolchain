#include "DTIDEGLWidgets.h"
#include "DTIDEGLWidget.h"
#include <QDebug>

DTIDEGLWidgets::DTIDEGLWidgets(): QObject()
{
}

QGLWidget* DTIDEGLWidgets::requestWidget(QString title, int width, int height)
{
    QGLFormat glFormat(QGL::SampleBuffers);
    glFormat.setSwapInterval(0);
    
    // apparently only works in single buffer mode !?
    glFormat.setDoubleBuffer(false);
    
    QGLWidget* res = new DTIDEGLWidget(glFormat);
    
    res->resize(width, height);
    res->setWindowTitle(title);
    res->show();
    activeWidgets.append(res);

    res->makeCurrent();
    return res;
}

void DTIDEGLWidgets::killWidgets()
{
    while (!activeWidgets.isEmpty())
    {
        QGLWidget* w = activeWidgets.takeFirst();
        emit killDockWidget(w);

        w->close();
    }
}
