#include <iostream>
#include <string>
using namespace std;

// Подключаем наш класс
#include "pentaract_gateway.cpp"

int main() {
    cout << "🚀 Запуск теста Pentaract Gateway" << endl;
    cout << "==================================" << endl;
    
    // Создаём объект
    PentaractGateway gateway;
    
    // 1. Входим в систему
    if (gateway.login("admin@ranepa.ru", "SecurePass123!")) {
        
        // 2. Создаём хранилище
        string storage_id = gateway.create_storage("ranepa-schedule");
        
        if (!storage_id.empty()) {
            // 3. Сохраняем тестовое расписание
            string test_schedule = "{"
                "\"group\": \"2ИСОСП\","
                "\"lectures\": ["
                    "{\"subject\": \"Технологии программирования\", \"time\": \"09:00\"}"
                "]"
            "}";
            
            if (gateway.save_schedule(storage_id, "2ИСОСП", test_schedule)) {
                
                // 4. Загружаем обратно
                string loaded = gateway.get_schedule(storage_id, "2ИСОСП");
                cout << "\n📋 Загруженное расписание:" << endl;
                cout << loaded << endl;
            }
        }
    }
    
    cout << "\n🏁 Тест завершён!" << endl;
    return 0;
}