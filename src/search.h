#pragma once

#include <string>
#include <iostream>

class Filter{
public:
    enum eAttributeProc { NONE, PID, PPID, STATE, COMM, START };
    eAttributeProc getAttribute(void) const;
    eAttributeProc convert(const std::string& str);

private:
    eAttributeProc mAttributeProc;
};