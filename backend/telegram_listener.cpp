#include <iostream>
#include <string>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <unistd.h>

using namespace std;
using json = nlohmann::json;

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int main() {
    string bot_token = "8470518497:AAEbUQP9efjCSuRvZjbt_2CA99LQe80VpNk";
    string get_updates_url = "https://api.telegram.org/bot" + bot_token + "/getUpdates";
    string send_message_url = "https://api.telegram.org/bot" + bot_token + "/sendMessage";
    
    long long last_update_id = 0;
    long long your_chat_id = 5282212117; // Твой chat_id
    
    cout << "👂 Слушатель Telegram запущен..." << endl;
    cout << "Отправь боту команду /get_random чтобы получить данные" << endl;
    
    while (true) {
        CURL* curl = curl_easy_init();
        string response;
        
        // Получаем обновления
        string url = get_updates_url + "?offset=" + to_string(last_update_id + 1);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        
        CURLcode res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            try {
                json updates = json::parse(response);
                if (updates["ok"] && updates["result"].is_array()) {
                    for (auto& update : updates["result"]) {
                        last_update_id = max(last_update_id, update["update_id"].get<long long>());
                        
                        if (update.contains("message") && 
                            update["message"].contains("text") && 
                            update["message"]["text"] == "/get_random") {
                            
                            long long chat_id = update["message"]["chat"]["id"];
                            cout << "📨 Получена команда от chat_id: " << chat_id << endl;
                            
                            // Читаем последний файл из Pentaract (упрощённо)
                            // В реальности нужно делать запрос к Pentaract API
                            string fake_db = "{\"random_number\": 42, \"timestamp\": 1705600000}";
                            
                            // Отправляем ответ
                            json message_data;
                            message_data["chat_id"] = chat_id;
                            message_data["text"] = "🎲 Случайное число из базы: " + 
                                to_string(json::parse(fake_db)["random_number"].get<int>());
                            
                            string send_response;
                            struct curl_slist* headers = nullptr;
                            headers = curl_slist_append(headers, "Content-Type: application/json");
                            
                            curl_easy_setopt(curl, CURLOPT_URL, send_message_url.c_str());
                            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, message_data.dump().c_str());
                            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
                            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &send_response);
                            
                            curl_easy_perform(curl);
                            curl_slist_free_all(headers);
                            
                            cout << "📤 Отправлен ответ пользователю" << endl;
                        }
                    }
                }
            } catch (const exception& e) {
                cerr << "❌ Ошибка парсинга: " << e.what() << endl;
            }
        }
        
        curl_easy_cleanup(curl);
        sleep(2); // Проверяем каждые 2 секунды
    }
    
    return 0;
}