#include <iostream>
#include <string>
#include "http_client.cpp"  // Включаем наш HTTP-клиент
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

class PentaractAPI {
private:
    string base_url;
    string auth_token;
    HttpClient client;

public:
    PentaractAPI(const string& url = "http://localhost:8000") : base_url(url) {}

    // Авторизация
    bool login(const string& email, const string& password) {
        cout << "🔑 Авторизация в Pentaract..." << endl;
        
        json auth_data;
        auth_data["email"] = email;
        auth_data["password"] = password;

        CurlResponse response = client.post(base_url + "/api/v1/auth/token", auth_data);
        
        if (response.status_code == 200) {
            try {
                json result = json::parse(response.data);
                auth_token = result["access_token"].get<string>();
                cout << "✅ Успешный вход!" << endl;
                return true;
            } catch (const exception& e) {
                cerr << "❌ Ошибка парсинга JSON: " << e.what() << endl;
            }
        } else {
            cerr << "❌ Ошибка авторизации. Код: " << response.status_code << endl;
            cerr << "Ответ: " << response.data << endl;
        }
        return false;
    }

    // Создание хранилища
    string create_storage(const string& name) {
        if (auth_token.empty()) {
            cerr << "❌ Нет токена авторизации!" << endl;
            return "";
        }

        cout << "📁 Создаём хранилище: " << name << endl;
        json storage_data;
        storage_data["name"] = name;

        CurlResponse response = client.post(base_url + "/api/v1/storages", storage_data, auth_token);
        
        if (response.status_code == 201) {
            try {
                json result = json::parse(response.data);
                string storage_id = result["id"].get<string>();
                cout << "✅ Хранилище создано: " << storage_id << endl;
                return storage_id;
            } catch (const exception& e) {
                cerr << "❌ Ошибка парсинга создания хранилища: " << e.what() << endl;
            }
        } else {
            cerr << "❌ Ошибка создания хранилища. Код: " << response.status_code << endl;
            cerr << "Ответ: " << response.data << endl;
        }
        return "";
    }

    // Загрузка расписания
    bool upload_schedule(const string& storage_id, const string& group_id, const json& schedule_data) {
        if (auth_token.empty()) {
            cerr << "❌ Нет токена авторизации!" << endl;
            return false;
        }

        cout << "📤 Загружаем расписание для группы: " << group_id << endl;
        
        // Сохраняем как файл в хранилище
        CurlResponse response = client.post(
            base_url + "/api/v1/storages/" + storage_id + "/files", 
            schedule_data, 
            auth_token
        );
        
        if (response.status_code == 201) {
            cout << "✅ Расписание успешно загружено!" << endl;
            return true;
        } else {
            cerr << "❌ Ошибка загрузки. Код: " << response.status_code << endl;
            cerr << "Ответ: " << response.data << endl;
            return false;
        }
    }

    // Получение расписания
    json get_schedule(const string& storage_id, const string& group_id) {
        if (auth_token.empty()) {
            cerr << "❌ Нет токена авторизации!" << endl;
            return json::object();
        }

        cout << "📥 Загружаем расписание для группы: " << group_id << endl;
        
        CurlResponse response = client.get(
            base_url + "/api/v1/storages/" + storage_id + "/files",
            auth_token
        );
        
        if (response.status_code == 200) {
            try {
                json result = json::parse(response.data);
                cout << "✅ Расписание получено!" << endl;
                return result;
            } catch (const exception& e) {
                cerr << "❌ Ошибка парсинга расписания: " << e.what() << endl;
            }
        } else {
            cerr << "❌ Ошибка получения расписания. Код: " << response.status_code << endl;
            cerr << "Ответ: " << response.data << endl;
        }
        return json::object();
    }
};