#include <QtGui>

#include "dtidesplash.h"
#include "mainwindow.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    DTIDESplash splash;
    if(!splash.exec())
        return 0;

    ProjectProperties p = splash.getProperties();
    
    DTIDE mainWindow(p);
    mainWindow.show();

    return app.exec();
}
