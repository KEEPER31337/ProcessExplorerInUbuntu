#include <iostream>
#include <string>
#include <sstream>
#include "processinfo.h"

using namespace std;

string specificinfo(int pid){
    char *cmd;

    sprintf(cmd, "cat /proc/%d/stat", pid);

    FILE *stream = popen(cmd, "r");

    ostringstream output;

    while( !feof(stream) && !ferror(stream)){
        char *buf;
        int bytesRead = fread(buf, 1, 128, stream);
        output.write(buf, bytesRead);
    }
    
    string result = output.str();
    
    return result;
}