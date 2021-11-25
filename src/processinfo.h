#include <vector>
#include <string>

#pragma once

using namespace std;

struct ProcInfo
{   
    int pid;
    int ppid;
    char state;     
    string comm;    //�봽濡쒖꽭�뒪 �씠由�
    string start;   //start time
};