#include "processinfo.h"
#include "sysinfo.h"
//#include "search.h"

#pragma once

using namespace std;

typedef unsigned long long ULL;

class Command
{
public:
    enum Mode
    {
        PRINTPROCINFO,
        CLEAR
    };
    Command();
    // setting print mode
    void SetMode(Mode m);
    Mode GetMode(void) const;

    vector<ProcInfo> &GetProcInfos(void) const;
    void UpdateProcStat(void);

    // shell command functions
    string GetHelp(string cmdFunc);
    ProcInfo GetProcInfoByPID(int pid);
    vector<ProcInfo> &GetProcInfoByPPID(int ppid);
    void SendSignal(int pid, int signalNum);
    vector<ProcInfo> &SearchProc(vector<ProcInfo> &procInfo, string procAttr, string proc);
    string GetProcPath(int pid);

private:
    // for update proc status
    void setSysInfo(void);
    double getCpuTime(ULL utime, ULL stime, ULL starttime, int seconds);
    string getStartTime(ULL uptime, ULL stime);
    string getUserName(char* path);

    SysInfo *mSysInfo;
    vector<ProcInfo> *mProcInfo;
    Mode mMode;
};