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
        ss << "PID : "      << pid      << ", "
           << "PPID : "     << ppid     << ", ";
        ss.precision(1);
        ss << fixed
           << "CPU : "      << cpu      << ", "
           << "MEM : "      << vmem     << ", "
           << "STAT : "     << state    << ", "
           << "USER : "     << user     << ", "
           << "RUNTIME : "  << start    << ", "
           << "THREADS : "  << threads  << ", "
           << "COMMAND : "  << comm     << ", "
        ;

        return ss.str();
    }

    string procInfoToStr(void)
    {
        stringstream ss;
        ss << setw(8)  << pid  
           << setw(8)  << ppid;
        ss.precision(1);
        ss << fixed
           << setw(8)  << cpu  
           << setw(12) << vmem 
           << setw(8)  << state
           << setw(8)  << user 
           << setw(10) << start
           << setw(8)  << threads 
           << setw(20) << comm 
        ;

        return ss.str();
    }

    int     pid;        // process id
    int     ppid;       // parent process id
    string  comm;       // command
    double  cpu;        // cpu 사용률
    ULL     vmem;       // 메모리 점유율
    char    state;      // state
    string  user;       // 프로세스 사용 유저명
    string  start;      // 프로세스 시작 시간
    int     threads;    // # of threads (number of light weight process)
    int     uid;        // user id
    int     euid;       // effective user id
};