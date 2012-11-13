#ifndef DTIDEGLWIDGET_H
#define DTIDEGLWIDGET_H

#include <QGLWidget>
#include <QGLContext>

class DTIDEGLWidget: public QGLWidget
{
    Q_OBJECT

public:
    DTIDEGLWidget();
    DTIDEGLWidget(QGLFormat & context);
 protected:

     void initializeGL();

     void resizeGL(int w, int h);

     void paintGL();
};

#endif
