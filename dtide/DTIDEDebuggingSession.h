#ifndef DTIDEDEBUGGINGSESSION_H
#define DTIDEDEBUGGINGSESSION_H

#include "Backends.h"

class DTIDEDebuggingSession: public DebuggingSession
{
public:
    DTIDEDebuggingSession();

    void Stop();
    void Break(int id, std::string filename, int line);
    void AddError(int id, std::string message);
    void AddWarning(int id, std::string message);

    void SetMemory(uint16_t memory[0x10000]);
    void PushCall(std::string call, std::string language, std::string file, int line);
    void PopCall(std::string language);

    void SetActiveDevice(int devid, Device& device);

    bool HasMessages();
    virtual void AddMessage(DebuggingMessage m);
    DebuggingMessage GetMessage();

    virtual void AddBreakpoint(uint16_t address);
};

#endif
