#include "processinfo.h"
#include "sysinfo.h"
#include "search.h"

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
    string Help(void);
    ProcInfo GetProcInfoByPID(int PID);
    void SendSignal(int PID, int signalNum);
    void RestartProc(int PID, string procPath);
    vector<ProcInfo> &SearchProc(vector<ProcInfo>& procInfo, std::string procAttr, std::string proc, Command &cmd);


private:
    // for update proc status
    void SetSysInfo(void);
    double GetCpuTime(ULL utime, ULL stime, ULL starttime, int seconds);
    string GetStartTime(ULL uptime, ULL stime);
    string GetUserName(char* path);

    SysInfo *mSysInfo;
    vector<ProcInfo> *mProcInfo;
    Mode mMode;
};