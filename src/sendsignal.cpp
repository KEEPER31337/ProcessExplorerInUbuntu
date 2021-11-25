#include <stdio.h>
#include <stdlib.h>

void SendSignal(int PID, int signalNum)
{

    char *killCMD;
    
    sprintf(killCMD, "kill -%d %d", signalNum, PID); //PID客 signal可记阑 持篮 kill疙飞绢 积己

    system(killCMD);

}