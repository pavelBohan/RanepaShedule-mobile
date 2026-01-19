#include <microhttpd.h>
#include <string>
#include <iostream>
#include "pentaract_api.cpp" // Твой рабочий клиент

using namespace std;

// Глобальный клиент Pentaract (авторизован при старте)
PentaractAPI* g_pentaract = nullptr;
string g_storage_id = "0e5825e4-d35b-4e42-96bf-1e52a34e53cc"; // Твой ID

// Обработчик HTTP-запросов
int request_handler(void* cls, struct MHD_Connection* connection,
                   const char* url, const char* method,
                   const char* version, const char* upload_data,
                   size_t* upload_data_size, void** ptr) {
    
    if (string(method) != "GET") {
        return MHD_NO;
    }

    // Пример: GET /schedule?group=2ИСОСП
    string group = "2ИСОСП"; // Извлеки из параметров URL

    // Получаем расписание из Pentaract
    json schedule = g_pentaract->get_schedule(g_storage_id, group);
    string response = schedule.dump();

    // Формируем HTTP-ответ
    struct MHD_Response* mhd_response = MHD_create_response_from_buffer(
        response.length(), (void*)response.c_str(), MHD_RESPMEM_MUST_COPY
    );
    MHD_add_response_header(mhd_response, "Content-Type", "application/json");
    MHD_add_response_header(mhd_response, "Access-Control-Allow-Origin", "*");

    int ret = MHD_queue_response(connection, MHD_HTTP_OK, mhd_response);
    MHD_destroy_response(mhd_response);
    return ret;
}

int main() {
    cout << "🚀 Запуск локального сервера на http://localhost:8080" << endl;
    
    // Авторизуемся в Pentaract
    PentaractAPI api("http://localhost:8000");
    if (!api.login("admin@ranepa.ru", "wsWCFOpecBQlryaw")) {
        cerr << "❌ Не удалось авторизоваться в Pentaract!" << endl;
        return 1;
    }
    g_pentaract = &api;

    // Запускаем HTTP-сервер
    struct MHD_Daemon* daemon = MHD_start_daemon(
        MHD_USE_THREAD_PER_CONNECTION, 8080, NULL, NULL,
        &request_handler, NULL, MHD_OPTION_END
    );

    if (daemon == NULL) {
        cerr << "❌ Не удалось запустить сервер!" << endl;
        return 1;
    }

    cout << "✅ Сервер запущен! Открой в браузере: http://localhost:8080/schedule" << endl;
    cout << "📱 С телефона: http://<ТВОЙ_IP>:8080/schedule" << endl;

    // Ждём нажатия Enter
    cin.get();

    MHD_stop_daemon(daemon);
    return 0;
}