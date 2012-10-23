#include "DTIDEGLWidgets.h"

DTIDEGLWidgets::DTIDEGLWidgets(): QObject()
{
}

QGLWidget* DTIDEGLWidgets::requestWidget(QString title, int width, int height)
{
    QGLFormat glFormat(QGL::SampleBuffers);
    glFormat.setSwapInterval(0);
    QGLWidget* res = new QGLWidget();
    res->setFormat(glFormat);

    emit spawnGLWidget(res, title, width, height);

    return res;
}
