/**

    File:           ErrorList.h

    Project:        DCPU-16 Tools
    Component:      LibDCPU-ci-lang

    Authors:        Patrick Flick

    Description:    Declares the ErrorList class.

**/

#ifndef __DCPU_LIBDCC_ERRORLIST_H
#define __DCPU_LIBDCC_ERRORLIST_H

#include <string>
#include <vector>
#include <iostream>

class Error
{
public:
    int line;
    std::string file;
    int errid;
    std::string errdata;
    int warnErr;
    
    Error(int line, std::string file, int errid, std::string errdata, int warnErr) :
        line(line),
        file(file),
        errid(errid),
        errdata(errdata),
        warnErr(warnErr) {}
};

class ErrorList
{
private:
    std::vector<Error> m_list;
    bool m_hasWarnings;
    bool m_hasErrors;
public:
    static const int ERROR_LIST_WARNING = 0;
    static const int ERROR_LIST_ERROR = 1;
    static const int ERROR_LIST_FATAL_ERROR = 2;

    ErrorList() : m_list(std::vector<Error>()), m_hasWarnings(false), m_hasErrors(false) {}
    
    void addWarning(int line, std::string file, int errid);
    void addWarning(int line, std::string file, int errid, std::string errdata);
    void addError(int line, std::string file, int errid);
    void addError(int line, std::string file, int errid, std::string errdata);
    void addFatalError(int line, std::string file, int errid);
    void addFatalError(int line, std::string file, int errid, std::string errdata);
    
    bool hasWarnings();
    bool hasErrors();
    
    void printall();
};

#endif
