#include <iostream>
#include <stdlib.h>

using namespace std;

string Help()
{
    
    string manual =  "\npath [PID/PROCESS_NAME] : this command display the path of each process\n\n\
    viruscheck [PID/PROCESS_NAME] : this command check a status which process is infection\n\n\
    info [PID/PROCESS_NAME]: this command display the information of processs' detail informain\n\n\
    restart [PID/PROCESS_NAME] : this command restarts the process\n\n\
    filter [PID/PROCESS_NAME] : this command display filtered specific processes\n\n\
    \n";

    return manual;
}