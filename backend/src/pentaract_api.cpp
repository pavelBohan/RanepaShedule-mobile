#include <iostream>
#include <string>
#include "http_client.cpp"
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

    // ПРАВИЛЬНЫЙ ЭНДПОИНТ АВТОРИЗАЦИИ
    bool login(const string& email, const string& password) {
        cout << "🔑 Авторизация в Pentaract..." << endl;
        
        json auth_data;
        auth_data["email"] = email;
        auth_data["password"] = password;

        // ИСПРАВЛЕНО: /auth/token вместо /auth/login
        CurlResponse response = client.post(base_url + "/api/auth/login", auth_data);
        
        if (response.status_code == 200) {
            try {
                json result = json::parse(response.data);
                // В Pentaract токен находится в поле "access_token"
                auth_token = result["access_token"].get<string>();
                cout << "✅ Успешный вход!" << endl;
                return true;
            } catch (const exception& e) {
                cerr << "❌ Ошибка парсинга JSON: " << e.what() << endl;
                cerr << "Ответ сервера: " << response.data << endl;
            }
        } else {
            cerr << "❌ Ошибка авторизации. Код: " << response.status_code << endl;
            cerr << "Ответ сервера: " << response.data << endl;
        }
        return false;
    }

        string create_storage(const string& name, long long chat_id) {
        if (auth_token.empty()) {
            cerr << "❌ Нет токена авторизации!" << endl;
            return "";
        }

        cout << "📁 Создаём хранилище: " << name << " (chat_id: " << chat_id << ")" << endl;
        json storage_data;
        storage_data["name"] = name;
        storage_data["chat_id"] = chat_id; // ОБЯЗАТЕЛЬНОЕ ПОЛЕ

        CurlResponse response = client.post(base_url + "/api/storages", storage_data, auth_token);
        
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
            cerr << "Ответ сервера: " << response.data << endl;
        }
        return "";
    }

    bool upload_schedule(const string& storage_id, const string& group_id, const json& schedule_data) {
        if (auth_token.empty()) {
            cerr << "❌ Нет токена авторизации!" << endl;
            return false;
        }

        cout << "📤 Загружаем расписание для группы: " << group_id << endl;
        
        // Конвертируем JSON в строку
        string json_str = schedule_data.dump();
        
        // Создаём временный файл
        string filename = "schedule_" + group_id + ".json";
        ofstream file(filename);
        if (file.is_open()) {
            file << json_str;
            file.close();
        } else {
            cerr << "❌ Не удалось создать временный файл!" << endl;
            return false;
        }

        // Формируем multipart/form-data вручную (упрощённо)
        string boundary = "----CurlBoundary7MA4YWxkTrZu0gW";
        string body = "--" + boundary + "\r\n"
            "Content-Disposition: form-data; name=\"file\"; filename=\"" + filename + "\"\r\n"
            "Content-Type: application/json\r\n\r\n"
            + json_str + "\r\n"
            "--" + boundary + "--\r\n";

        // Отправляем запрос
        CurlResponse response = client.post_multipart(
            base_url + "/api/storages/" + storage_id + "/files",
            body,
            boundary,
            auth_token
        );
        
        if (response.status_code == 201) {
            cout << "✅ Расписание успешно загружено!" << endl;
            // Удаляем временный файл
            remove(filename.c_str());
            return true;
        } else {
            cerr << "❌ Ошибка загрузки. Код: " << response.status_code << endl;
            cerr << "Ответ сервера: " << response.data << endl;
            remove(filename.c_str());
            return false;
        }
    }

    json get_schedule(const string& storage_id, const string& group_id) {
        if (auth_token.empty()) {
            cerr << "❌ Нет токена авторизации!" << endl;
            return json::object();
        }

        cout << "📥 Загружаем расписание для группы: " << group_id << endl;
        
        CurlResponse response = client.get(
            base_url + "/api/storages/" + storage_id + "/files",
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
            cerr << "Ответ сервера: " << response.data << endl;
        }
        return json::object();
    }
};