#include "DTIDEDebuggingSession.h"
#include <iostream>

DTIDEDebuggingSession::DTIDEDebuggingSession()
{
}

void DTIDEDebuggingSession::Stop()
{
}

void DTIDEDebuggingSession::Break(int id, std::string filename, int line)
{
}

void DTIDEDebuggingSession::AddError(int id, std::string message)
{
}

void DTIDEDebuggingSession::AddWarning(int id, std::string message)
{
}

void DTIDEDebuggingSession::SetMemory(uint16_t memory[0x10000])
{
}

void DTIDEDebuggingSession::PushCall(std::string call, std::string language, std::string file, int line)
{
}

void DTIDEDebuggingSession::PopCall(std::string language)
{
}

void DTIDEDebuggingSession::SetActiveDevice(int devid, Device& device)
{
}

bool DTIDEDebuggingSession::HasMessages()
{
    return m_Queue.size() > 0;
}

void DTIDEDebuggingSession::AddMessage(DebuggingMessage m)
{
    m_Queue.push_back(m);
}

DebuggingMessage DTIDEDebuggingSession::GetMessage()
{
    DebuggingMessage result = m_Queue.front();
    m_Queue.pop_front();

    return result;
} 
