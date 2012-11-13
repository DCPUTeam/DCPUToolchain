#include "DTIDEGLWidget.h"
#include <QDebug>

DTIDEGLWidget::DTIDEGLWidget(Toolchain* toolchain, void* ud): QGLWidget()
{
    this->setAutoBufferSwap(false);
    this->m_toolchain = toolchain;
    this->m_userData = ud;
}

DTIDEGLWidget::DTIDEGLWidget(Toolchain* toolchain, QGLFormat & context, void* ud): QGLWidget(context)
{
    this->setAutoBufferSwap(false);
    this->m_toolchain = toolchain;
    this->m_userData = ud;
}

void DTIDEGLWidget::initializeGL()
{
}

void DTIDEGLWidget::resizeGL(int w, int h)
{
    this->m_toolchain->RelayResize(w, h, this->m_userData);
}

void DTIDEGLWidget::paintGL()
{
}
