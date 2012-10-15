#include <QtGui>

#include <list>

#include "backends.h"
#include "dtidesplash.h"
#include "mainwindow.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    std::list<Toolchain*> toolchains;
    toolchains.insert(toolchains.end(), new DCPUToolchain());
    
/*    DTIDESplash* splash = new DTIDESplash(toolchains);
    if(!splash->exec())
        return 0;
   
    DTIDE mainWindow(splash->toolchain, splash->fileName);*/

    DTIDE mainWindow(new DCPUToolchain(), "lol.dasm");
    mainWindow.show();

    return app.exec();

    return 0;
}

