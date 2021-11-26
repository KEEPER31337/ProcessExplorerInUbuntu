#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

#pragma once

using namespace std;

typedef unsigned long long ULL;

struct ProcInfo {
    string procInfoToStrWithName(void)
    {
        stringstream ss;
        ss << "PID : "   << pid     << ", "
        << "PPID : "     << ppid    << ", "
        << "COMMAND : "  << comm    << ", "
        << "CPU : "      << cpu     << ", "
        << "MEM : "      << vmem    << ", "
        << "STAT : "     << state   << ", "
        << "USER : "     << user    << ", "
        << "START : "    << start   << ", "
        << "THREADS : "  << nlwp    << ", "
        //<< "PNAME : "    << name    << ", "
        ;

        return ss.str();
    }

    string procInfoToStr(void)
    {
        stringstream ss;
        ss << setw(8)  << pid  
           << setw(8)  << ppid 
           << setw(20) << comm 
           << setw(8)  << cpu  
           << setw(12) << vmem 
           << setw(5)  << state
           << setw(8)  << user 
           << setw(8)  << start
           << setw(8)  << nlwp 
           //<< setw(20) << name 
        ;

        return ss.str();
    }

    int     pid;        // process id
    int     ppid;       // parent process id
    string  comm;       // command
    double  cpu;        // cpu 사용률
    int     vmem;       // 메모리 점유율
    char    state;      // state
    string  user;       // 프로세스 사용 유저명
    string  start;      // 프로세스 시작 시간
    int     nlwp;       // # of threads (number of light weight process)
    //string  name;       // process name
};