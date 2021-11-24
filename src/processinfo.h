#include <vector>
#include <string>

#pragma once

using namespace std;

struct ProcInfo
{   
    int pid;
    int ppid;
    char state;
    string comm;    //프로세스 이름
    string start;  
};