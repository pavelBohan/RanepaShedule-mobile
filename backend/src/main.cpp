#include <iostream>
#include <string>
#include "pentaract_api.cpp"

using namespace std;

int main() {
    cout << "🚀 Тест интеграции с Pentaract" << endl;
    cout << "================================" << endl;

    // Создаём API-клиент
    PentaractAPI api("http://localhost:8000");

    // 1. Авторизация (используй свои данные из .env)
    if (api.login("admin@ranepa.ru", "SecurePass123!")) {
        
        // 2. Создаём хранилище
        string storage_id = api.create_storage("ranepa-schedule");
        
        if (!storage_id.empty()) {
            
            // 3. Подготавливаем тестовое расписание
            nlohmann::json test_schedule;
            test_schedule["group"] = "2ИСОСП";
            test_schedule["lectures"] = nlohmann::json::array();
            
            nlohmann::json lecture;
            lecture["subject"] = "Технологии программирования";
            lecture["teacher"] = "Безбородов Н.";
            lecture["time"] = "09:00-10:30";
            lecture["classroom"] = "ауд. 412";
            test_schedule["lectures"].push_back(lecture);

            // 4. Загружаем расписание
            if (api.upload_schedule(storage_id, "2ИСОСП", test_schedule)) {
                
                // 5. Получаем обратно
                auto loaded = api.get_schedule(storage_id, "2ИСОСП");
                cout << "\n📋 Полученное расписание:" << endl;
                cout << loaded.dump(2) << endl;
            }
        }
    }
    
    cout << "\n🏁 Тест завершён!" << endl;
    return 0;
}