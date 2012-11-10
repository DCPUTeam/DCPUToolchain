#include "Backends.h"
#include "Toolchains/Toolchains.h"

std::list<std::string> BuildAPI::GetOutputFiles()
{
    return m_OutputFiles;
}

std::list<std::string> BuildAPI::GetSymbolFiles()
{
    return m_SymbolFiles;
}

bool DebuggingSession::BreakpointAt(uint16_t address)
{
    std::list<uint16_t>::iterator it;
    for(it = m_BreakList.begin(); it != m_BreakList.end(); it++)
    {
        if(*it == address)
            return true;
    }
    
    return false;
}

Line DebuggingSession::GetLine()
{
    return m_CurrentLine;
}

void DebuggingSession::SetCurrentLine(Line l)
{
    m_CurrentLine = l;
}

Language* Toolchain::GetLanguageByExtension(std::string ext)
{
    std::list<Language*>::iterator it;
    std::list<Language*> langs = GetLanguages();
    for (it = langs.begin(); it != langs.end(); it++)
    {
        std::list<std::string> exts = dynamic_cast<Language*>(*it)->GetExtensions();
        std::list<std::string>::iterator it_ext;

        for (it_ext = exts.begin(); it_ext != exts.end(); it_ext++)
        {
            if (*it_ext == ext)
                return *it;
        }
    }

    return NULL;
}

void Toolchain::SetWidgetFactory(DTIDEGLWidgets* w)
{
    gl = w;
}

bool Toolchain::IsPaused()
{
    return paused;
}
