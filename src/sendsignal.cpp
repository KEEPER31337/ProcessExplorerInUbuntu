#include <stdio.h>
#include <stdlib.h>

void SendSignal(int pid, int signalnum){

    char *kill_cmd;
    
    sprintf(kill_cmd, "kill -%d %d", signalnum, pid); //PID客 signal可记阑 持篮 kill疙飞绢 积己

    system(kill_cmd);

}