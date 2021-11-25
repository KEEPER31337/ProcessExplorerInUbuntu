#include "processinfo.h"

#pragma once

using namespace std;

class Command {
public:
    enum Mode { PRINTPROCINFO , CLEAR };
    Command();
    int getProcNum(void);
    vector<ProcInfo> &getProcInfo(void) const;
    void updateProcStat(void);
    void printProc(void) const;
    void setMode(Mode m);
    Mode getMode(void) const;
    string Help(void);
    ProcInfo GetProcInfoByPID(vector<ProcInfo> procInfo,int PID);
    void SendSignal(int PID, int signalnum);

private:
    vector<ProcInfo> *mProcInfo;
    Mode mMode;
};