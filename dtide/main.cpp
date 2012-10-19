#include <QtGui>

#include <list>

#include "backends.h"
#include "dtidesplash.h"
#include "mainwindow.h"

extern "C"
{
#include <osutil.h>
#include <bstring.h>
#include <debug.h>
#include <iio.h>
}

int main(int argc, char** argv)
{
    // Some set up required for toolchain to work.
	debug_setlevel(LEVEL_VERBOSE);
	osutil_setarg0(bautofree(bfromcstr(argv[0])));
	isetmode(IMODE_BIG);

    // Now start the IDE.
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

