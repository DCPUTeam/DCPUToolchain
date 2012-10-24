#ifndef DTIDEGLWIDGETS_H
#define DTIDEGLWIDGETS_H

#include <QGLWidget>

class DTIDEGLWidgets: public QObject
{
    Q_OBJECT

public:
    DTIDEGLWidgets();

    QGLWidget* requestWidget(QString title, int width, int height);
    void killWidgets();

signals:
    void spawnGLWidget(QGLWidget*, QString, int, int);
    void killDockWidget(QGLWidget*);

private:
    QList<QGLWidget*> activeWidgets;
};

#endif
