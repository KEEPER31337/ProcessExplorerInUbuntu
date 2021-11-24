#ifndef REQUESTE_HPP
#define REQUESTR_HPP
#include <cstring>
#include <curl/curl.h>
#include <iostream>
#include <memory>
#include <string>

using namespace std;

namespace Request {
     const string VIRUS_TOTAL_REPORT_URL = "https://www.virustotal.com/vtapi/v2/file/report?";
     namespace
     {
         static size_t writer(char *, size_t, size_t, string *);
     }
     static unique_ptr<CURL *> curl_obj;

     string set_url(const string&, const string &);
     void get_data(const string)  noexcept(false);
     unique_ptr<string> get_write_buffer();
}

#endif