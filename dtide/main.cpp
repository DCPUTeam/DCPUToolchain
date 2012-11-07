#include <QtGui>

#include <list>

#include "Backends.h"
#include "DTIDESplash.h"
#include "DTIDEXMLProject.h"
#include "DTIDE.h"

extern "C"
{
#include <osutil.h>
#include <bstring.h>
#include <debug.h>
#include <iio.h>
#include <version.h>
}

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    // Some set up required for toolchain to work.
    debug_setlevel(LEVEL_VERBOSE);
    osutil_setarg0(bautofree(bfromcstr(argv[0])));
    isetmode(IMODE_BIG);
    
    // Show version information.
    version_print(bautofree(bfromcstr("IDE")));

    // Project management.
    DTIDEXMLProject* p = new DTIDEXMLProject();
    std::list<Toolchain*> toolchains;
    toolchains.insert(toolchains.end(), new DCPUToolchain());

    if(argc == 2)
        p->read(argv[1]);
    else
        p->read("project.xml");

    if(!p->loaded)
    {

        DTIDESplash* splash = new DTIDESplash(toolchains);
        if(!splash->exec())
            return 0;

        p->setTitle(splash->projectTitle);
        p->addFile(splash->fileName, true);
        p->setToolchain(new DCPUToolchain());
    }

    // Now start the IDE.
    DTIDE mainWindow(p);
    mainWindow.show();

    int returncode = app.exec();
    return returncode;
}

