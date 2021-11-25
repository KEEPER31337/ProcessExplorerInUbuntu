#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>
#include <fstream>
#include <openssl/bio.h>
#include <openssl/hmac.h>
#include <openssl/buffer.h>
#include <curl/curl.h>

#include "json/json.h"

typedef struct fileData {
    char *data;
    unsigned long long length;
} fileData;

const std::string VIRUSTOTAL_URL = "https://www.virustotal.com/vtapi/v2/";
const std::string VIRUSTOTAL_SCAN_URL = "https://www.virustotal.com/vtapi/v2/file/scan";
const std::string APIKEY = "717e5ae38a274a05ecebbdb08291a3350fdbde97b29b1ff03666c4d95d543a3a";

fileData &getFileData(std::string &fileName)
{
    std::ifstream ifs(fileName, std::ifstream::binary);
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

std::string &base64(fileData &filedata)
{
    std::string *s = new std::string();
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

static size_t writeMemory(char *data, size_t size, size_t nmemb, std::string *s)
{
    if (data == NULL)
        return 0;

    s->append(data, size * nmemb);

    return size * nmemb;
}

std::string &makeScanPostFields(std::string &fileName)
{
    fileData filedata = getFileData(fileName);

    std::string *postFields = new std::string();
    *postFields = "apikey=";
    *postFields += APIKEY;
    *postFields += "&file=data:application/octet-stream;";
    *postFields += "name=hashtest;";
    *postFields += "base64,";
    *postFields += base64(filedata);

    return *postFields;
}

std::string &makeReportGetURL(std::string &resource)
{
    std::string *url = new std::string();

    *url  = VIRUSTOTAL_URL;
    *url += "file/report";
    *url += "?apikey=";
    *url += APIKEY;
    *url += "&resource=";
    *url += resource;
    *url += "&allinfo=false";

    return *url;
}

std::string &requestReport(std::string resource)
{
    CURL *curl;
    CURLcode code;
    std::string *s = new std::string();

    curl = curl_easy_init();
    if (curl == NULL)
    {
        std::cerr << "curl init error" << std::endl;
    }
    code = curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
    if (code != CURLE_OK)
        std::cerr << "CustomRequest Error : " << curl_easy_strerror(code) << std::endl;

    code = curl_easy_setopt(curl, CURLOPT_URL, makeReportGetURL(resource).c_str());
    if (code != CURLE_OK)
        std::cerr << "URL Error : " << curl_easy_strerror(code) << std::endl;

    code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeMemory);
    if (code != CURLE_OK)
        std::cerr << "WriteFunction Error : " << curl_easy_strerror(code) << std::endl;

    code = curl_easy_setopt(curl, CURLOPT_WRITEDATA, s);
    if (code != CURLE_OK)
        std::cerr << "WriteData Error : " << curl_easy_strerror(code) << std::endl;

    code = curl_easy_perform(curl);
    if (code != CURLE_OK)
        std::cerr << "curl_easy_perform Error : " << curl_easy_strerror(code) << std::endl;

    curl_easy_cleanup(curl);

    return *s;
}

std::string &requestScan(std::string &fileName)
{
    CURL *curl;
    CURLcode code;
    std::string *s = new std::string();

    curl = curl_easy_init();
    if( curl == NULL ) {
        std::cerr << "curl init error" << std::endl;
    }
    code = curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
    if ( code != CURLE_OK )
        std::cerr << "CustomRequest Error : " << curl_easy_strerror(code) << std::endl;

    code = curl_easy_setopt(curl, CURLOPT_URL, VIRUSTOTAL_SCAN_URL.c_str());
    if ( code != CURLE_OK )
        std::cerr << "URL Error : " << curl_easy_strerror(code) << std::endl;
    struct curl_slist *headers = NULL;

    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded");

    code = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    if ( code != CURLE_OK )
        std::cerr << "HttpHeader Error : " << curl_easy_strerror(code) << std::endl;

    code = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, makeScanPostFields(fileName).c_str());
    if ( code != CURLE_OK )
        std::cerr << "PostFields Error : " << curl_easy_strerror(code) << std::endl;

    code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeMemory);
    if ( code != CURLE_OK )
        std::cerr << "WriteFunction Error : " << curl_easy_strerror(code) << std::endl;
    
    code = curl_easy_setopt(curl, CURLOPT_WRITEDATA, s);
    if ( code != CURLE_OK )
        std::cerr << "WriteData Error : " << curl_easy_strerror(code) << std::endl;

    code = curl_easy_perform(curl);
    if ( code != CURLE_OK )
        std::cerr << "curl_easy_perform Error : " << curl_easy_strerror(code) << std::endl;

    curl_easy_cleanup(curl);

    return *s;
}

Json::Value &parsingJson(std::string &data)
{
    Json::Value *rootr = new Json::Value();
    Json::Reader reader;

    if ( !reader.parse(data, *rootr) ) {
        std::cerr << "json parse error" << std::endl;
    }

    return *rootr;
}



int main(int argc, char *argv[])
{
    std::string s;
    Json::Value scanResult;
    Json::Value reportResult;
    Json::Value scanList;
    std::string fileName;
    int detectedCnt = 0;
    int totalCnt    = 0;

    if (argc == 1) {
        std::cerr << "please Input FileName." << std::endl;
        exit(1);
    } else if (argc != 2) {
        std::cerr << "please Input 1 argument." << std::endl;
        exit(1);
    } else {
        fileName = argv[1];
        s = requestScan(fileName);
    }
    // std::cout << s << std::endl;

    scanResult = parsingJson(s);

    s = requestReport(scanResult["resource"].asCString());
    scanList = parsingJson(s)["scans"];
    totalCnt = scanList.size();
    for (Json::ValueIterator it = scanList.begin(); it != scanList.end(); it++) {
        if ((*it)["detected"].asString() != "false") {
            detectedCnt++;
        }
    }
    std::cout << detectedCnt << '/' << totalCnt << '\n';
    // reportResult = parsingJson(s);
    return 0;
}