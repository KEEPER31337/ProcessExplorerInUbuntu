#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>
#include <stdlib.h>

using namespace std;

void Restart(int pid, string path){

    char kill_cmd[1024];
    char run_cmd[1024];
    
    sprintf(kill_cmd, "kill -9 %d", pid);
    strcpy(run_cmd, path.c_str());

    system(kill_cmd);
    system(run_cmd);

}