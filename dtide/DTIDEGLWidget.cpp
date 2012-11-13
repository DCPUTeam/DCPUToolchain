#include "DTIDEGLWidget.h"
#include <QDebug>

DTIDEGLWidget::DTIDEGLWidget(): QGLWidget()
{
    this->setAutoBufferSwap(false);
}

DTIDEGLWidget::DTIDEGLWidget(QGLFormat & context): QGLWidget(context)
{
}

void DTIDEGLWidget::initializeGL()
{
}

void DTIDEGLWidget::resizeGL(int w, int h)
{
}

void DTIDEGLWidget::paintGL()
{
}
