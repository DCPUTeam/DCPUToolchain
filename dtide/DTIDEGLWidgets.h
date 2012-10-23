#ifndef DTIDEGLWIDGETS_H
#define DTIDEGLWIDGETS_H

#include <QGLWidget>

class DTIDEGLWidgets: public QObject
{
    Q_OBJECT

public:
    DTIDEGLWidgets();

    QGLWidget* requestWidget(QString title, int width, int height);

signals:
    void spawnGLWidget(QGLWidget*, QString, int, int);
};

#endif
