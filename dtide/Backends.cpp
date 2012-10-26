#include "Backends.h"
#include "Toolchains/Toolchains.h"

Language* Toolchain::GetLanguageByExtension(std::string ext)
{
    std::list<Language*>::iterator it;
    std::list<Language*> langs = GetLanguages();
    for(it = langs.begin(); it != langs.end(); it++)
    {
        std::list<std::string> exts = dynamic_cast<Language*>(*it)->GetExtensions();
        std::list<std::string>::iterator it_ext;

        for(it_ext = exts.begin(); it_ext != exts.end(); it_ext++)
        {
            if(*it_ext == ext)
            return *it;
        }
    }

    return NULL;
}

void Toolchain::SetWidgetFactory(DTIDEGLWidgets* w)
{
    gl = w;
}
