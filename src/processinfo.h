#include <vector>
#include <string>

#pragma once

using namespace std;

struct ProcInfo {       
    int     pid;        // process id
    int     ppid;       // parent process id
    string  comm;       // command
    double  cpu;        // cpu 사용률
    int     vmem;       // 메모리 점유율
    char    state;      // state
    string  user;   // 프로세스 사용 유저명
    string  start;      // 프로세스 시작 시간
    int     nlwp;       // # of threads (number of light weight process)
    string  name;       // process name
};