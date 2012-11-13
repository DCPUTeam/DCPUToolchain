#ifndef DTIDEGLWIDGET_H
#define DTIDEGLWIDGET_H

#include <QGLWidget>
#include <QGLContext>
class Toolchain;
#include "Backends.h"

class DTIDEGLWidget: public QGLWidget
{
    Q_OBJECT
private:
    Toolchain* m_toolchain;
    void* m_userData;
public:
    DTIDEGLWidget(Toolchain* toolchain, void* ud);
    DTIDEGLWidget(Toolchain* toolchain, QGLFormat & context, void* ud);
 protected:

     void initializeGL();

     void resizeGL(int w, int h);

     void paintGL();
};

#endif
