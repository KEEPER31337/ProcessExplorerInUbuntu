#include "processinfo.h"
#include "json/json.h"

#pragma once

using namespace std;

typedef unsigned long      UL;
typedef unsigned long long ULL;

typedef struct fileData {
    char *data;
    unsigned long long length;
} fileData;

const string VIRUSTOTAL_URL = "https://www.virustotal.com/vtapi/v2/";
const string VIRUSTOTAL_SCAN_URL = "https://www.virustotal.com/vtapi/v2/file/scan";
const string APIKEY = "717e5ae38a274a05ecebbdb08291a3350fdbde97b29b1ff03666c4d95d543a3a";

class Command
{
public:
    enum Mode
    {
        PRINTPROCINFO,
        CLEAR,
        EXIT,
    };
    Command();
    // setting print mode
    void SetMode(Mode m);
    Mode GetMode(void) const;

    vector<ProcInfo> &GetProcInfos(void) const;
    void UpdateProcStat(void);

    // shell command functions
    string GetHelp(string cmdFunc);
    ProcInfo GetProcInfoByPID(int pid);
    vector<ProcInfo> *GetProcInfoByPPID(int ppid);
    void SendSignal(int pid, int signalNum);
    vector<ProcInfo> *SearchProc(vector<ProcInfo> &procInfo, string procAttr, string proc);
    string GetProcPath(int pid);
    string GetVirusTotalReport(string filepath);

private:
    void    sortProcInfos(bool(*cmpFunc)(ProcInfo&, ProcInfo&));
    // for update proc status
    double  getUptime(void);
    double  getCpuTime(UL utime, UL stime, ULL starttime);
    string  getRunTime(ULL stime);

    //for virus total
    fileData &getFileData(string &fileName);
    string &base64(fileData &filedata);
    
    string &makeScanPostFields(string &fileName);
    string &makeReportGetURL(string &resource);
    string &requestReport(string resource);
    string &requestScan(string &fileName);
    Json::Value &parsingJson(string &data);

    vector<ProcInfo> *mProcInfos;
    Mode mMode;
};

static size_t writeMemory(char *data, size_t size, size_t nmemb, string *s);
bool compareByCPU(ProcInfo &p1, ProcInfo &p2);