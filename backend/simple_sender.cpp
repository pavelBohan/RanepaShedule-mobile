#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

// Callback для CURL
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int main() {
    // Генерируем случайное число
    srand(time(0));
    int random_number = rand() % 1000;
    
    cout << "🎲 Сгенерировано случайное число: " << random_number << endl;
    
    // Авторизуемся в Pentaract
    CURL* curl = curl_easy_init();
    string auth_url = "http://localhost:8000/api/auth/login";
    json auth_data;
    auth_data["email"] = "admin@ranepa.ru";
    auth_data["password"] = "wsWCFOpecBQlryaw";
    
    string auth_response;
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    
    curl_easy_setopt(curl, CURLOPT_URL, auth_url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, auth_data.dump().c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &auth_response);
    
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        cerr << "❌ Ошибка авторизации: " << curl_easy_strerror(res) << endl;
        return 1;
    }
    
    // Парсим токен
    json auth_result = json::parse(auth_response);
    string token = auth_result["access_token"];
    cout << "✅ Авторизация успешна!" << endl;
    
    // Создаём данные для отправки
    json db_data;
    db_data["random_number"] = random_number;
    db_data["timestamp"] = time(nullptr);
    
    // Отправляем в хранилище
    string storage_id = "0e5825e4-d35b-4e42-96bf-1e52a34e53cc"; // Твой ID
    string upload_url = "http://localhost:8000/api/storages/" + storage_id + "/files";
    
    string upload_response;
    headers = curl_slist_append(headers, ("Authorization: Bearer " + token).c_str());
    
    curl_easy_setopt(curl, CURLOPT_URL, upload_url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, db_data.dump().c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &upload_response);
    
    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        cerr << "❌ Ошибка загрузки: " << curl_easy_strerror(res) << endl;
        return 1;
    }
    
    cout << "📤 Данные отправлены в Pentaract!" << endl;
    cout << "📄 Ответ: " << upload_response << endl;
    
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return 0;
}