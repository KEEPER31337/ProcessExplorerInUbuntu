#include <iostream>
#include <sstream>
#include <vector>
#include <string>

#include "processinfo.h"
#include "search.h"
#include "command.h"
#include "find.h"

bool FindByChar(const char tmp, const char str);
bool FindByStr(const string tmp, const string str);

Filter::eAttributeProc Filter::getAttribute(void) const
{
    return mAttributeProc;
}

Filter::eAttributeProc Filter::convert(const std::string& str)
{
    if (str == "PID") return PID;
    else if (str == "PPID") return PPID;
    else if (str == "STATE") return STATE;
    else if (str == "COMM") return COMM;
    else if (str == "START") return START;
    else return NONE;
}

vector<ProcInfo> &SearchProc(vector<ProcInfo>& procInfo, std::string procAttr, std::string proc, Command &cmd)
{
    vector<ProcInfo> *resultProc;
    Filter *conv;
    
    resultProc = new vector<ProcInfo>;

    vector<ProcInfo>::iterator ptr = procInfo.begin();
    const std::string attr = procAttr;
    switch(conv->convert(attr)){
        case Filter::eAttributeProc::PID :
            const int pid = stoi(proc);
            resultProc->push_back(cmd.GetProcInfoByPID(pid));
            break;
        case Filter::eAttributeProc::PPID :
            const int ppid = stoi(proc);
            resultProc->push_back(cmd.GetProcInfoByPID(ppid));
            break;
        case Filter::eAttributeProc::STATE :
            for(ptr; ptr != procInfo.end(); ++ptr)
            {
                const char state = *proc.c_str();
                const char procState = ptr->state;
                if(Find::FindByChar(state, procState)) resultProc->push_back(*ptr);
            }
            break;
        case Filter::eAttributeProc::COMM :
            for(ptr; ptr != procInfo.end(); ++ptr)
            {
                const std::string procName = proc;
                const std::string procComm = ptr->comm;
                if(Find::FindByStr(procName, procComm)) resultProc->push_back(*ptr);
            }
            break;
        case Filter::eAttributeProc::START :
            for(ptr; ptr != procInfo.end(); ++ptr)
            {
                const std::string startTime = proc;
                const std::string procTime = ptr->start;
                if(Find::FindByStr(startTime, procTime)) resultProc->push_back(*ptr);
            }
            break;
        default :
            return *resultProc;
    }

    return *resultProc;
}