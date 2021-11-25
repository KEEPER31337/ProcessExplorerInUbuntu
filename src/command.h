#include "processinfo.h"
#include "sysinfo.h"

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
    vector<ProcInfo> &GetProcInfos(void) const;
    void UpdateProcStat(void);
    void SetMode(Mode m);
    Mode GetMode(void) const;
    string Help(void);
    ProcInfo GetProcInfoByPID(vector<ProcInfo> procInfo,int PID);
    void SendSignal(int PID, int signalNum);
    void RestartProc(int PID, string procPath);
    
    void PrintProc(void) const; //출력 테스트 목적

private:
    SysInfo *msysinfo;
    vector<ProcInfo> *mProcInfo;
    Mode mMode;
    void SetSysInfo(void);
    double GetCpuTime(ULL utime, ULL stime, ULL starttime, int seconds);
    string GetStartTime(ULL uptime, ULL stime);
    string GetUserName(char* path);

};