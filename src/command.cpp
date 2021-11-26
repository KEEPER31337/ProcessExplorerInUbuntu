#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <pwd.h>
#include <string>
#include <cstring>
#include <vector>
#include <signal.h>
#include <sstream>
#include <limits.h>
#include <algorithm>

// virustotal
#include <iomanip>
#include <openssl/bio.h>
#include <openssl/hmac.h>
#include <openssl/buffer.h>
#include <curl/curl.h>
#include "json/json.h"

// for debug
#include <cassert>

#include "processinfo.h"
#include "command.h"
#include "search.h"

using namespace std;

typedef unsigned long long ULL;

Command::Command()
    : mMode(PRINTPROCINFO)
{
    mSysInfo = new SysInfo();
    setSysInfo();
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
            ps.user = upasswd->pw_name;

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
            
            ps.pid      = stoul(t[0]);
            ps.ppid     = stoul(t[3]);
            ps.state    = t[2][0];
            ps.comm     = t[1];
            ps.cpu      = getCpuTime(stoul(t[13]), stoul(t[14]), stoul(t[21]), mSysInfo->uptime);
            ps.start    = getStartTime(mSysInfo->uptime, stoul(t[21]));
            ps.vmem     = stoul(t[22]);

            mProcInfo->push_back(ps);

            ifs.close();
        }
    }
    if (closedir(dp) == -1) {
        cerr << "dir close error" << endl;
    }
}

void Command::setSysInfo(void) //sysinfo의 값 설정
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

double Command::getCpuTime(ULL utime, ULL stime, ULL starttime, int seconds) //CPU 점유율
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

string Command::getStartTime(ULL uptime, ULL stime) //문자열 포맷 형식에 따라 프로세스 시작시간을 문자열로 반환하는 함수
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

string Command::getUserName(char *filepath)
{
    struct passwd *upasswd;
    struct stat lstat;

    if (stat(filepath, &lstat))
    {
        upasswd = getpwuid(lstat.st_uid);
        string str(upasswd->pw_name);
        return str;
    }
    else
    {
        cerr << filepath << " is not valid." << endl;
        return NULL;
    }
}

void Command::SendSignal(int pid, int signalNum)
{
    kill(pid, signalNum);
}

ProcInfo Command::GetProcInfoByPID(int pid)
{
    ProcInfo selecProc;
    UpdateProcStat();
    vector<ProcInfo>::iterator it;
    for(it = mProcInfo->begin(); it != mProcInfo->end(); ++it)
    {
        if( it->pid == pid ) {
            selecProc = *it;
            break;
        }
    }
    if( it == mProcInfo->end() )
        selecProc.pid = -1;
    return selecProc;
}
vector<ProcInfo> *Command::GetProcInfoByPPID(int ppid)
{
    vector<ProcInfo> *result;
    result = new vector<ProcInfo>;

    vector<ProcInfo>::iterator it;
    for(it = mProcInfo->begin(); it != mProcInfo->end(); ++it) {
        if(it->ppid == ppid) {
            result->push_back(*it);
        }
    }

    return result;
}

vector<ProcInfo> *Command::SearchProc(vector<ProcInfo> &procInfo, string kind, string proc)
{
    vector<ProcInfo> *resultProc;    
    resultProc = new vector<ProcInfo>;

    vector<ProcInfo>::iterator it;
    transform(kind.begin(), kind.end(),kind.begin(), ::toupper);

    if (kind == "PID") {
        const int pid = stoi(proc);
        resultProc->push_back(GetProcInfoByPID(pid));
    }
    else if (kind == "PPID") {
        const int ppid = stoi(proc);
        resultProc = GetProcInfoByPPID(ppid);
    }
    else if (kind == "STATE") {
        for( it = procInfo.begin(); it != procInfo.end(); ++it ) {
            const char state = proc.c_str()[0];
            const char procState = it->state;
            if( state == procState ) resultProc->push_back(*it);
        }
    }
    else if (kind == "COMM") {
        for( it = procInfo.begin(); it != procInfo.end(); ++it ) {
            const string procName = proc;
            const string procComm = it->comm;
            if(procName == procComm) resultProc->push_back(*it);
        }
    }
    else if (kind == "START") {
        for( it = procInfo.begin(); it != procInfo.end(); ++it ) {
            const string startTime = proc;
            const string procTime = it->start;
            if(startTime == procTime) resultProc->push_back(*it);
        }
    }
    else {
        return NULL;
    }

    return resultProc;
}

string Command::GetProcPath(int pid)
{
    char buf[PATH_MAX];
    ssize_t len;

    stringstream ss;
    ss << "/proc/" << pid << "/exe"; 

    len = readlink(ss.str().c_str(), buf, sizeof(buf)-1);
    if ( len == -1 ) {
        return string("cannot get path");
    }
    buf[len] = '\0';
    return string(buf);
}

// for virustotal
string Command::GetVirusTotalReport(int pid)
{
    stringstream result;
    string filepath;
    Json::Value scanResult;
    Json::Value reportResult;
    Json::Value scanList;
    string fileName;
    int detectedCnt = 0;
    int totalCnt    = 0;

    filepath = GetProcPath(pid);
    scanResult = parsingJson( requestScan(filepath) );
    scanList = parsingJson( requestReport(scanResult["resource"].asCString()) )["scans"];
    totalCnt = scanList.size();

    for (Json::ValueIterator it = scanList.begin(); it != scanList.end(); it++) {
        if ((*it)["detected"].asString() != "false") {
            detectedCnt++;
        }
    }

    result << "Virus Detected(detected / total) : "
           << detectedCnt << " / " << totalCnt;

    return result.str();
}

fileData &Command::getFileData(string &fileName)
{
    ifstream ifs(fileName, ifstream::binary);
    fileData *filedata = new fileData();
    int length;

    ifs.seekg(0, ifs.end);
    length = ifs.tellg();
    ifs.seekg(0, ifs.beg);

    filedata->data = new char[length];
    filedata->length = length;

    ifs.read(filedata->data, length);

    return *filedata;
}

string &Command::base64(fileData &filedata)
{
    string *s = new string();
    BIO *bmem, *b64;
    BUF_MEM *bptr;

    b64 = BIO_new(BIO_f_base64());
    bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);

    BIO_write(b64, filedata.data, filedata.length);
    BIO_flush(b64);
    BIO_get_mem_ptr(b64, &bptr);

    *s = bptr->data;

    return *s;
}

static size_t writeMemory(char *data, size_t size, size_t nmemb, string *s)
{
    if (data == NULL)
        return 0;

    s->append(data, size * nmemb);

    return size * nmemb;
}

string &Command::makeScanPostFields(string &fileName)
{
    fileData filedata = getFileData(fileName);

    string *postFields = new string();
    *postFields = "apikey=";
    *postFields += APIKEY;
    *postFields += "&file=data:application/octet-stream;";
    *postFields += "name=hashtest;";
    *postFields += "base64,";
    *postFields += base64(filedata);

    return *postFields;
}

string &Command::makeReportGetURL(string &resource)
{
    string *url = new string();

    *url  = VIRUSTOTAL_URL;
    *url += "file/report";
    *url += "?apikey=";
    *url += APIKEY;
    *url += "&resource=";
    *url += resource;
    *url += "&allinfo=false";

    return *url;
}

string &Command::requestReport(string resource)
{
    CURL *curl;
    CURLcode code;
    string *s = new string();

    curl = curl_easy_init();
    if (curl == NULL)
    {
        cerr << "curl init error" << endl;
    }
    code = curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
    if (code != CURLE_OK)
        cerr << "CustomRequest Error : " << curl_easy_strerror(code) << endl;

    code = curl_easy_setopt(curl, CURLOPT_URL, makeReportGetURL(resource).c_str());
    if (code != CURLE_OK)
        cerr << "URL Error : " << curl_easy_strerror(code) << endl;

    code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeMemory);
    if (code != CURLE_OK)
        cerr << "WriteFunction Error : " << curl_easy_strerror(code) << endl;

    code = curl_easy_setopt(curl, CURLOPT_WRITEDATA, s);
    if (code != CURLE_OK)
        cerr << "WriteData Error : " << curl_easy_strerror(code) << endl;

    code = curl_easy_perform(curl);
    if (code != CURLE_OK)
        cerr << "curl_easy_perform Error : " << curl_easy_strerror(code) << endl;

    curl_easy_cleanup(curl);

    return *s;
}

string &Command::requestScan(string &fileName)
{
    CURL *curl;
    CURLcode code;
    string *s = new string();

    curl = curl_easy_init();
    if( curl == NULL ) {
        cerr << "curl init error" << endl;
    }
    code = curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
    if ( code != CURLE_OK )
        cerr << "CustomRequest Error : " << curl_easy_strerror(code) << endl;

    code = curl_easy_setopt(curl, CURLOPT_URL, VIRUSTOTAL_SCAN_URL.c_str());
    if ( code != CURLE_OK )
        cerr << "URL Error : " << curl_easy_strerror(code) << endl;
    struct curl_slist *headers = NULL;

    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");

    code = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    if ( code != CURLE_OK )
        cerr << "HttpHeader Error : " << curl_easy_strerror(code) << endl;

    code = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, makeScanPostFields(fileName).c_str());
    if ( code != CURLE_OK )
        cerr << "PostFields Error : " << curl_easy_strerror(code) << endl;

    code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeMemory);
    if ( code != CURLE_OK )
        cerr << "WriteFunction Error : " << curl_easy_strerror(code) << endl;
    
    code = curl_easy_setopt(curl, CURLOPT_WRITEDATA, s);
    if ( code != CURLE_OK )
        cerr << "WriteData Error : " << curl_easy_strerror(code) << endl;

    code = curl_easy_perform(curl);
    if ( code != CURLE_OK )
        cerr << "curl_easy_perform Error : " << curl_easy_strerror(code) << endl;

    curl_easy_cleanup(curl);

    return *s;
}

Json::Value &Command::parsingJson(string &data)
{
    Json::Value *rootr = new Json::Value();
    Json::Reader reader;

    if ( !reader.parse(data, *rootr) ) {
        cerr << "json parse error" << endl;
    }

    return *rootr;
}