#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>
#include <stdlib.h>

void RestartProc(int PID, std::string pathProc)
{

    char *killCMD;
    char *runCMD;
    
    sprintf(killCMD, "kill -9 %d", PID);
    strcpy(runCMD, pathProc.c_str());

    system(killCMD);
    system(runCMD);

}