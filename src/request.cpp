#include "request.hpp"

using namespace std;

const string apikey = "60c411cb1d050512547331684c62a89fff7160b1c9a6f6d6f5d58158c03f941b";

namespace request {

    static array<char, CURL_ERROR_SIZE> error_buffer;
    static string wbuffer;

    static size_t writer(char *data, size_t size, size_t nmemb, string *writer_data){
        if(writer_data == nullptr) return 0;

        writer_data -> append(data, size * nmemb)l
        return size *nmemb;
    }

    string set_url(const string &api_key, const string &hash){
        return VIRUS_TOTAL_REPORT_URL + "apikey=" + api_key + "&resource=" + hash;
    }

    void get_data(const string url) noexcept(false)
    {
        CURLcode code;
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl_obj = make_unique<CURL *>(curl_easy_init());

        if(curl_obj == nullptr)
            throw runtime_error("Libcurl couldn't be initialized.");

        if((code = curl_easy_setopt(*curl_obj, CURLOPT_URL, url.c_str())) != CURLE_OK)
            throw runtime_error("Failed setting URL.");

        memset(&error_buffer, 0, CURL_ERROR_SIZE);
        if((code = curl_easy_setopt(*curl_obj, CURLOPT_ERRORBUFFER, error_buffer.data())) != CURLE_OK)
            throw runtime_error(string("CURLOPT_ERRORBUFFER: ") + error_buffer.data());

        if((code = curl_easy_setopt(*curl_obj, CURLOPT_WRITEFUNCTION, Requester::writer)) != CURLE_OK)
            throw runtime_error(string("CURLOPT_WRITEFUNCTION: ") + error_buffer.data());

        if((code = curl_easy_setopt(*curl_obj, CURLOPT_WRITEDATA, &wbuffer)) != CURLE_OK)
            throw runtime_error(string("CURLOPT_WRITEDATA: ") + error_buffer.data());

         if((code = curl_easy_perform(*curl_obj)) != CURLE_OK)
             throw runtime_error(string("curl_easy_perform: ") + error_buffer.data());

         curl_easy_cleanup(*curl_obj);
    }

    unique_ptr<string> get_write_buffer()
    {
        return make_unique<string>(wbuffer);
    }
}