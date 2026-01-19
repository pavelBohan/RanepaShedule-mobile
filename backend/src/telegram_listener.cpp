// Каждые 2 секунды опрашиваем Telegram
while (running) {
    auto updates = telegram_api.get_updates(last_update_id);
    
    for (auto& update : updates) {
        if (update.from.id == YOUR_CHAT_ID) { // Только твои команды
            string text = update.message.text;
            
            if (text.find("/schedule ") == 0) {
                string group = extract_group(text);
                json schedule = pentaract_core.get_schedule(group);
                
                // Отправляем ОТВЕТ обратно тебе через Telegram
                telegram_api.send_message(YOUR_CHAT_ID, format_json(schedule));
            }
        }
        last_update_id = max(last_update_id, update.update_id);
    }
    sleep(2);
}