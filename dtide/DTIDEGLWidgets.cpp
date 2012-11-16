#include "DTIDEGLWidgets.h"
#include "DTIDEGLWidget.h"
#include <QDebug>

DTIDEGLWidgets::DTIDEGLWidgets(): QObject()
{
}

DTIDEGLWidget* DTIDEGLWidgets::requestWidget(Toolchain* toolchain, QString title, int width, int height, void* ud)
{
    QGLFormat glFormat(QGL::SampleBuffers);
    glFormat.setSwapInterval(0);
    
    // apparently only works in single buffer mode !?
    glFormat.setDoubleBuffer(false);
    
    DTIDEGLWidget* res = new DTIDEGLWidget(toolchain, glFormat, ud);
    
    //res->resize(width, height);
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
