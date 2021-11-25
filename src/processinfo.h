#include <vector>
#include <string>

#pragma once

using namespace std;

struct ProcInfo
{   
    string username;    //프로세스 사용 유저명
    double cpu;   //cpu 사용률
    string start; //프로세스 시작 시간
    int vmem;//메모리 점유율
    
    int pid;
    int ppid;
    char state;
    string comm;    //프로세스 이름
     
};