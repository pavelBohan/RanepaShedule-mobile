#include <iostream>
#include <string>
#include <fstream>
#include <vector>
using namespace std;

// Простая функция для отправки HTTP-запроса (упрощённая)
// В реальном проекте здесь будет библиотека, но для начала так
string send_http_request(string url, string method, string body = "") {
    // Это заглушка! Позже заменим на настоящую HTTP-библиотеку
    cout << "📡 Отправляем " << method << " запрос на " << url << endl;
    if (!body.empty()) {
        cout << "📦 Тело запроса: " << body << endl;
    }
    return "{\"access_token\":\"fake_token_123\",\"id\":\"storage_456\"}";
}

class PentaractGateway {
private:
    string base_url = "http://localhost:8000";
    string auth_token = "";

public:
    // Авторизация в системе
    bool login(string email, string password) {
        cout << "🔑 Пытаемся войти в систему..." << endl;
        cout << "📡 Отправляем запрос на: " << base_url + "/api/v1/auth/token" << endl;
        
        // Создаём JSON для входа
        string login_data = "{"
            "\"email\": \"" + email + "\","
            "\"password\": \"" + password + "\""
        "}";
        
        // Отправляем запрос
        string response = send_http_request(base_url + "/api/v1/auth/login", "POST", login_data);
        
        // Проверяем ответ (в реальности нужно парсить JSON)
        if (response.find("access_token") != string::npos) {
            auth_token = "fake_token_123"; // В реальности берём из ответа
            cout << "✅ Успешный вход!" << endl;
            return true;
        }
        cout << "❌ Ошибка входа!" << endl;
        return false;
    }

    // Создание хранилища
    string create_storage(string name) {
        if (auth_token.empty()) {
            cout << "❌ Сначала нужно войти в систему!" << endl;
            return "";
        }
        
        cout << "📁 Создаём хранилище: " << name << endl;
        string storage_data = "{\"name\": \"" + name + "\"}";
        string response = send_http_request(base_url + "/api/v1/storages", "POST", storage_data);
        
        // Возвращаем ID хранилища (в реальности из ответа)
        return "storage_456";
    }

    // Сохранение расписания
    bool save_schedule(string storage_id, string group_name, string schedule_data) {
        if (auth_token.empty()) {
            cout << "❌ Сначала нужно войти в систему!" << endl;
            return false;
        }
        
        cout << "💾 Сохраняем расписание для группы " << group_name << endl;
        cout << "📂 В хранилище: " << storage_id << endl;
        
        // Сохраняем данные в файл (для теста)
        string filename = "schedule_" + group_name + ".json";
        ofstream file(filename);
        if (file.is_open()) {
            file << schedule_data;
            file.close();
            cout << "✅ Расписание сохранено в файл: " << filename << endl;
            return true;
        }
        cout << "❌ Не удалось сохранить файл!" << endl;
        return false;
    }

    // Получение расписания
    string get_schedule(string storage_id, string group_name) {
        if (auth_token.empty()) {
            cout << "❌ Сначала нужно войти в систему!" << endl;
            return "";
        }
        
        cout << "📥 Загружаем расписание для группы " << group_name << endl;
        
        // Читаем из файла (для теста)
        string filename = "schedule_" + group_name + ".json";
        ifstream file(filename);
        string content;
        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                content += line;
            }
            file.close();
            cout << "✅ Расписание загружено!" << endl;
            return content;
        }
        cout << "❌ Файл не найден!" << endl;
        return "";
    }
};