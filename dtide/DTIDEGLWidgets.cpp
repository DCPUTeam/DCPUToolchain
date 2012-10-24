#include "DTIDEGLWidgets.h"
#include <QDebug>

DTIDEGLWidgets::DTIDEGLWidgets(): QObject()
{
}

QGLWidget* DTIDEGLWidgets::requestWidget(QString title, int width, int height)
{
    QGLFormat glFormat(QGL::SampleBuffers);
    glFormat.setSwapInterval(0);
    QGLWidget* res = new QGLWidget();
    res->setFormat(glFormat);

    activeWidgets.append(res);

    emit spawnGLWidget(res, title, width, height);

    return res;
}

void DTIDEGLWidgets::killWidgets()
{
    while(!activeWidgets.isEmpty())
    {
        QGLWidget* w = activeWidgets.takeFirst();
        emit killDockWidget(w);

        delete w;
    }
}
