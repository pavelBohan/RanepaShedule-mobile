#include <iostream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

// Структура для ответа CURL
struct CurlResponse {
    string data;
    long status_code = 0;
};

// Callback для записи данных
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

class HttpClient {
private:
    CURL* curl;
    struct curl_slist* headers;

public:
    HttpClient() {
        curl = curl_easy_init();
        headers = nullptr;
        if (curl) {
            // Устанавливаем общие настройки
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        }
    }

    ~HttpClient() {
        if (headers) curl_slist_free_all(headers);
        if (curl) curl_easy_cleanup(curl);
    }

    // POST-запрос с JSON
    CurlResponse post(const string& url, const json& data, const string& auth_token = "") {
        if (!curl) {
            return {"CURL not initialized", 500};
        }

        string post_data = data.dump();
        string response_string;

        // Устанавливаем заголовки
        headers = curl_slist_append(headers, "Content-Type: application/json");
        if (!auth_token.empty()) {
            string auth_header = "Authorization: Bearer " + auth_token;
            headers = curl_slist_append(headers, auth_header.c_str());
        }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);

        CURLcode res = curl_easy_perform(curl);
        long status_code = 0;
        if (res == CURLE_OK) {
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);
        }

        // Очищаем заголовки для следующего запроса
        curl_slist_free_all(headers);
        headers = nullptr;

        return {response_string, status_code};
    }

    // GET-запрос
    CurlResponse get(const string& url, const string& auth_token = "") {
        if (!curl) {
            return {"CURL not initialized", 500};
        }

        string response_string;

        if (!auth_token.empty()) {
            string auth_header = "Authorization: Bearer " + auth_token;
            headers = curl_slist_append(headers, auth_header.c_str());
        }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);

        CURLcode res = curl_easy_perform(curl);
        long status_code = 0;
        if (res == CURLE_OK) {
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status_code);
        }

        curl_slist_free_all(headers);
        headers = nullptr;

        return {response_string, status_code};
    }
};