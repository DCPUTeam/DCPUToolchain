#include <QtGui>

#include "mainwindow.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    DTIDE mainWindow;
    mainWindow.show();

    return app.exec();
}
