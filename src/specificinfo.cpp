#include <iostream>
#include <string>
#include <sstream>
#include <stdio.h>
#include <stack>
#include <algorithm>

#include "processinfo.h"
#include "command.h"

ProcInfo Command::GetProcInfoByPID(int PID)
{
    ProcInfo selecProc;

    vector<ProcInfo>::iterator ptr;
    for(ptr = mProcInfo->begin(); ptr != mProcInfo->end(); ++ptr)
    {
        if(ptr->pid == PID) 
        {
            selecProc.pid = ptr->pid;
            selecProc.ppid = ptr->ppid;
            selecProc.state = ptr->state;
            selecProc.comm = ptr->comm;
            selecProc.start = ptr->start;
        }
        else if(ptr->ppid == PID) 
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