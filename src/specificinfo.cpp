#include <iostream>
#include <string>
#include <sstream>
#include <stdio.h>
#include "processinfo.h"

ProcInfo GetProcInfoByPID(vector<ProcInfo> procInfo, int PID)
{
    ProcInfo selecProc;

    vector<ProcInfo>::iterator ptr;
    for(ptr = procInfo.begin(); ptr != procInfo.end(); ++ptr)
    {
        if(ptr->pid == PID) 
        {
            selecProc.pid = ptr->pid;
            selecProc.ppid = ptr->ppid;
            selecProc.state = ptr->state;
            selecProc.comm = ptr->comm;
            selecProc.start = ptr->start;
        }
    }
    
    return selecProc;
}