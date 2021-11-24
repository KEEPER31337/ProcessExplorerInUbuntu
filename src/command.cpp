// opendir readdir
#include <dirent.h>

// chdir
#include <unistd.h>

// struct stat
#include <sys/stat.h>

// cerr
#include <iostream>
#include <fstream>
#include <stdlib.h>

#include <fcntl.h>
#include <sys/types.h>
#include <pwd.h>
#include <string>
#include <cstring>
#include <vector>

#include "processinfo.h"
#include "command.h"

using namespace std;
Command::Command()
    :mMode(PRINTPROCINFO)
{
    mProcInfo = new vector<ProcInfo>;
}

void Command::setMode(Mode m)
{
    mMode = m;
}
Command::Mode Command::getMode(void) const
{
    return mMode;
}

int Command::getProcNum()
{
    return mProcInfo->size();
}

vector<ProcInfo> &Command::getProcInfo(void) const{
    return *mProcInfo;
}

void Command::updateProcStat(void)
{
    DIR* dp;
    struct dirent *direntry;
    struct stat statbuf;

    if((dp=opendir("/proc"))==NULL){
        cerr<<"cannot open /proc dir"<<endl;
    }
    chdir("/proc");
    mProcInfo->clear();

    while((direntry=readdir(dp))!=NULL){
        lstat(direntry->d_name, &statbuf);
        if(S_ISDIR(statbuf.st_mode)&&isdigit(direntry->d_name[0])){
            ifstream ifs(strcat(direntry->d_name, "/stat"));
            if(!ifs.is_open()){
                cerr << "cannot open " << strcat(direntry->d_name,"/stat") << endl;
            }
            ProcInfo ps;
            string s;
            vector<string> t;
            for (int i=0;i<40;i++){
                ifs >> s;
                t.push_back(s);
            }

            ps.pid   = stoi(t[0]);
            ps.comm  = t[1];
            ps.state = t[2][0];
            ps.ppid  = stoi(t[3]);

            mProcInfo->push_back(ps);

            ifs.close();
        }
    }
    if ( closedir(dp) == -1 )
    {
        cerr << "dir close error" << endl;
    }
    
}
void Command::printProc(void) const 
{
    printf("%7s %7s %2s %16s\n", "PID", "PPID", "ST", "NAME"); 
    printf("===================================\n");
    for (int i = 0; i < mProcInfo->size(); i++){
        printf("%7d %7d %2c %16s\n",(*mProcInfo)[i].pid,
                                    (*mProcInfo)[i].ppid,
                                    (*mProcInfo)[i].state, 
                                    (*mProcInfo)[i].comm.c_str());
    }    
}
