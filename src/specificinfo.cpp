#include <iostream>
#include <string>
#include <sstream>
#include <stdio.h>
#include "processinfo.h"

ProcInfo getProcInfoByPID(vector<ProcInfo> procInfo, int pid)
{
    ProcInfo selecProc;

    vector<ProcInfo>::iterator ptr;
    for(ptr = procInfo.begin(); ptr != procInfo.end(); ++ptr)
    {
        if(ptr->pid == pid) 
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