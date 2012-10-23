#include "DTIDEGLWidgets.h"

DTIDEGLWidgets::DTIDEGLWidgets(): QObject()
{
}

QGLWidget* DTIDEGLWidgets::requestWidget(QString title, int width, int height)
{
    QGLWidget* res = new QGLWidget();
    emit spawnGLWidget(res, title, width, height);

    return res;
}
