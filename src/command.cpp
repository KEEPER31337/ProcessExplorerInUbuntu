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
#include <ctime>

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

typedef unsigned long       UL;
typedef unsigned long long  ULL;

Command::Command()
    : mMode(PRINTPROCINFO)
{
    mProcInfos = new vector<ProcInfo>;
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
    return *mProcInfos; 
}

void Command::UpdateProcStat(void)
{
    ifstream ifs;
    DIR *dp;
    struct dirent *direntry;
    struct stat statbuf;

    if ( (dp = opendir("/proc")) == NULL ) {
        cerr << "cannot open /proc dir" << endl;
        return ;
    }
    chdir("/proc");
    mProcInfos->clear();

    while ((direntry = readdir(dp)) != NULL) {
        
        if ( lstat(direntry->d_name, &statbuf) == -1 ) {
            // cerr << "lstat error : " << direntry->d_name << endl;
            continue;
        }

        if (S_ISDIR(statbuf.st_mode) && isdigit(direntry->d_name[0])) {
            string procDirPath = direntry->d_name;
            ProcInfo ps;
            string s;

            /* parse /proc/pid/stat file */
            ifs.open( procDirPath+string("/stat") );
            if ( !ifs.is_open() ) {
                // if cannot open, just skip
                // keep below code for debug
                //cerr << "cannot open " << procDirPath+string("/stat") << endl;
                continue;
            }
            
            vector<string> t; // /proc/[pid]/stat의 내용 파싱
            for ( int i = 0; i < 40; i++ ) {
                ifs >> s;
                t.push_back(s);
            }
            
            ps.pid   = stoi(t[0]);
            ps.state = t[2][0];
            ps.ppid  = stoi(t[3]);
            ps.cpu   = getCpuTime( stoul(t[13]), stoul(t[14]), stoull(t[21]) );
            ps.start = getRunTime( stoull(t[21]) );
            ps.vmem  = stoull(t[22]) / 1024;    // KB 단위

            ifs.close();

            /* parse /proc/pid/stat file */
            ifs.open( procDirPath+string("/status") );
            if ( !ifs.is_open() ) {
                //cerr << "cannot open " << procDirPath+string("/status") << endl;
                continue;
            }

            while ( ifs >> s ) {
                if( s.compare("Name:") == 0 ) {
                    ifs >> ps.comm;
                }
                if( s.compare("Uid:") == 0) {
                    ifs >> ps.uid;
                    ifs >> ps.euid;
                    ps.user = getpwuid(ps.euid)->pw_name;
                }
                if( s.compare("Threads:") == 0) {
                    ifs >> ps.threads;
                }
            }
            ifs.close();

            mProcInfos->push_back(ps);
        }
    }
    if (closedir(dp) == -1) {
        //cerr << "dir close error" << endl;
    }
    sortProcInfos(compareByCPU);
}

double Command::getUptime(void)
{
    double uptime;

    ifstream ifs("/proc/uptime");
    if ( !ifs.is_open() ) {
        cerr << "cannot open /proc/uptime" << endl;
        return 0;
    }
    ifs >> uptime;
    ifs.close();

    return uptime;
}

double Command::getCpuTime(UL utime, UL stime, ULL starttime) //CPU 점유율
{
    UL totalTime;
    double cpuTime;
    double cpuUsage;

    totalTime = utime + stime;
    cpuTime = getUptime() - (double) (starttime / sysconf(_SC_CLK_TCK));
    cpuUsage = (double) 100 * ( (totalTime / sysconf(_SC_CLK_TCK))  / cpuTime );

    return cpuUsage;
}

string Command::getRunTime(ULL starttime)
{
    stringstream ss;
    time_t runTime;
    int sec;
    int min;
    int hour;

    runTime = (ULL)getUptime() - starttime / sysconf(_SC_CLK_TCK);
    hour = runTime / (60 * 60);
    min = runTime % (60 * 60) / 60;
    sec = runTime % 60;
    
    ss << setw(2) << setfill('0') 
       << hour << ':' << setw(2) << setfill('0') 
       << min  << ':' << setw(2) << setfill('0')
       << sec;

    return ss.str();
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
    for(it = mProcInfos->begin(); it != mProcInfos->end(); ++it)
    {
        if( it->pid == pid ) {
            selecProc = *it;
            break;
        }
    }
    if( it == mProcInfos->end() )
        selecProc.pid = -1;
    return selecProc;
}
vector<ProcInfo> *Command::GetProcInfoByPPID(int ppid)
{
    vector<ProcInfo> *result;
    result = new vector<ProcInfo>;

    vector<ProcInfo>::iterator it;
    for(it = mProcInfos->begin(); it != mProcInfos->end(); ++it) {
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

void Command::sortProcInfos(bool(*cmpFunc)(ProcInfo&, ProcInfo&))
{
    sort(mProcInfos->begin(), mProcInfos->end(), cmpFunc);
}

bool compareByCPU(ProcInfo &p1, ProcInfo &p2)
{
    if( p1.cpu == p2.cpu )
        return p1.pid > p2.pid;
    return p1.cpu >= p2.cpu;
}

///////////////////////////
// for virustotal
///////////////////////////
string Command::GetVirusTotalReport(string filepath)
{
    stringstream result;
    Json::Value scanResult;
    Json::Value reportResult;
    Json::Value scanList;
    string fileName;
    int detectedCnt = 0;
    int totalCnt    = 0;

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