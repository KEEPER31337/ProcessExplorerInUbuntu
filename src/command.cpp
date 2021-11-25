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

// for debug
#include <cassert>

#include "processinfo.h"
#include "command.h"

using namespace std;

typedef unsigned long long ULL;

Command::Command()
    : mMode(PRINTPROCINFO)
{
    mSysInfo = new SysInfo();
    SetSysInfo();
    mProcInfo = new vector<ProcInfo>;
}

void Command::SetMode(Mode m) 
{ 
    mMode = m; 
}
Command::Mode Command::GetMode(void) const 
{ 
    return mMode; 
}

vector<ProcInfo> &Command::GetProcInfos(void) const 
{ 
    return *mProcInfo; 
}

void Command::UpdateProcStat(void)
{
    DIR *dp;
    struct dirent *direntry;
    struct stat statbuf;

    if ( (dp = opendir("/proc")) == NULL ) {
        cerr << "cannot open /proc dir" << endl;
    }
    chdir("/proc");
    mProcInfo->clear();

    while ((direntry = readdir(dp)) != NULL) {
        
        lstat(direntry->d_name, &statbuf);

        if (S_ISDIR(statbuf.st_mode) && isdigit(direntry->d_name[0])) {
            char pDir[20];    //현재 프로세스 파일명(pid)을 저장해둘 변수
            strcpy(pDir,direntry->d_name);
            ifstream ifs(strcat(direntry->d_name, "/stat"));
            if ( !ifs.is_open() )
            {
                // if cannot open, just skip
                // keep below code for debug
                // cerr << "cannot open " << strcat(direntry->d_name, "/stat") << endl;
                continue;
            }

            ProcInfo ps;
            string s;
            vector<string> t; // /proc/[pid]/stat의 내용 파싱
            for (int i = 0; i < 40; i++)
            {
                ifs >> s;
                t.push_back(s);
            }

            struct stat statfile;   // /[pid]/stat 파일의 정보를 담을 변수
            struct passwd *upasswd;
            stat(direntry->d_name,&statfile);   //statfile 변수에 값 할당
            upasswd = getpwuid(statfile.st_uid);
            ps.username = upasswd->pw_name;

            ifstream ifss(strcat(pDir,"/status"));
            if(!ifss.is_open()){    // status file exception
                continue;
            }
            string line;
            while(ifss>>s){
                getline(ifss,line);
                if(s.find("Name")!=string::npos){
                    ps.name=line;       //process name setting
                }
                if(s.find("Thread")!=string::npos){
                    ps.nlwp=stoi(line); //# of thread setting
                }
            }
            
            ps.pid      = stoi(t[0]);
            ps.ppid     = stoi(t[3]);
            ps.state    = t[2][0];
            ps.comm     = t[1];
            ps.cpu      = GetCpuTime(stoi(t[13]), stoi(t[14]), stoi(t[21]), mSysInfo->uptime);
            ps.start    = GetStartTime(mSysInfo->uptime, stoi(t[21]));
            ps.vmem     = stoi(t[22]);

            mProcInfo->push_back(ps);

            ifs.close();
        }
    }
    if (closedir(dp) == -1) {
        cerr << "dir close error" << endl;
    }
}

void Command::SetSysInfo(void) //sysinfo의 값 설정
{
    FILE *fp;
    double stime;
    double idlettime;

    ifstream ifs("/proc/uptime");
    if (!ifs.is_open())
    {
        cerr << "cannot open /proc/uptime" << endl;
    }

    ifs >> stime;
    ifs.close();
    mSysInfo->uptime = stime;
}

double Command::GetCpuTime(ULL utime, ULL stime, ULL starttime, int seconds) //CPU 점유율
{
    ULL total_time;
    int pcpu = 0;

    total_time = utime + stime;
    seconds = seconds - (int)(starttime / 100.);
    if (seconds)
    {
        pcpu = (int)(total_time * 1000ULL / 100.) / seconds;
    }
    return pcpu / 10.0;
}

string Command::GetStartTime(ULL uptime, ULL stime) //문자열 포맷 형식에 따라 프로세스 시작시간을 문자열로 반환하는 함수
{
    char result[16];
    unsigned int hertz = (unsigned int)sysconf(_SC_CLK_TCK);
    time_t startT = time(NULL) - (uptime - (stime / hertz));
    struct tm *tmStart = localtime(&startT);

    if (time(NULL) - startT < 24 * 60 * 60)
    {
        strftime(result, 16, "%H:%M", tmStart);
    }
    else if (time(NULL) - startT < 7 * 24 * 60 * 60)
    {
        strftime(result, 16, "%b %d", tmStart);
    }
    else
    {
        strftime(result, 16, "%y", tmStart);
    }

    string str(result);
    return str;
}
