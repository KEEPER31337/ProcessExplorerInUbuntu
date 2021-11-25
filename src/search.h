#pragma once

#include <string>
#include <iostream>

class Search{
public:
    enum eAttributeProc { NONE, PID, PPID, STATE, COMM, START };
    eAttributeProc getAttribute(const std::string& str);

private:
    eAttributeProc mAttributeProc;
};