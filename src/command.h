#include "processinfo.h"
#include "sysinfo.h"

#pragma once

using namespace std;

class Command
{
public:
    enum Mode
    {
        PRINTPROCINFO,
        CLEAR
    };
    Command();
    int getProcNum(void) const;
    vector<ProcInfo> &getProcInfo(void) const;
    void updateProcStat(void);
    double getCpuTime(ulong utime, ulong stime, ulong starttime, int seconds);
    string getStartTime(ulong uptime, ulong stime);
    void setSysInfo(void);
    void printProc(void) const;
    void setMode(Mode m);
    Mode getMode(void) const;

private:
    SysInfo *msysinfo;
    vector<ProcInfo> *mProcInfo;
    Mode mMode;
};