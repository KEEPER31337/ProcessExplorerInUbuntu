#include "processinfo.h"
#include "sysinfo.h"
#include "search.h"

#pragma once

using namespace std;

typedef unsigned long ul;

class Command
{
public:
    enum Mode
    {
        PRINTPROCINFO,
        CLEAR
    };
    Command();
    int GetProcNum(void) const; //�봽濡쒖꽭�뒪 珥� 媛쒖닔
    vector<ProcInfo> &GetProcInfos(void) const;
    void UpdateProcStat(void);
    void SetMode(Mode m);
    Mode GetMode(void) const;
    string Help(void);
    ProcInfo GetProcInfoByPID(int PID);
    void SendSignal(int PID, int signalNum);
    void RestartProc(int PID, string procPath);
    vector<ProcInfo> &SearchProc(vector<ProcInfo>& procInfo, std::string procAttr, std::string proc, Command &cmd);
    void PrintProc(void) const; //異쒕젰 �뀒�뒪�듃 紐⑹쟻

private:
    SysInfo *msysinfo;
    vector<ProcInfo> *mProcInfo;
    Mode mMode;
    void SetSysInfo(void);
    double GetCpuTime(ul utime, ul stime, ul starttime, int seconds);
    string GetStartTime(ul uptime, ul stime);
    string GetUserName(char* path);

};