#include "chat.h"
#include "../include/utils.h"
#include <iostream>

/**
 * Точка входа в приложение чата
 * Инициализирует сервисы и запускает веб-сервер
 */
int main() {
    // 1. Загрузка переменных окружения из .env файла
    auto env_vars = parse_env_file(".env");
    std::string jwt_secret = get_env_var("JWT_SECRET", env_vars);  // Секрет для JWT
    std::string db_file = get_env_var("DB_FILE", env_vars);       // Путь к файлу БД
    int port = std::stoi(get_env_var("PORT", env_vars, "8080"));  // Порт сервера (по умолчанию 8080)
    
    // Проверка обязательных переменных окружения
    if (jwt_secret.empty() || db_file.empty()) {
        std::cerr << "Error: Required environment variables not set" << std::endl;
        return 1;
    }
    
    // 2. Инициализация базы данных
    Database db(db_file);
    if (!db.initialize()) {
        std::cerr << "Failed to initialize database" << std::endl;
        return 1;
    }
    
    // 3. Инициализация сервиса аутентификации
    AuthService auth(db, jwt_secret);
    
    // 4. Создание Crow приложения
    crow::SimpleApp app;
    
    // 5. Инициализация чат-приложения и настройка маршрутов
    ChatApp chat_app(app, db, auth);
    chat_app.setup_routes();
    
    // 6. Запуск веб-сервера
    std::cout << "Server running on port " << port << std::endl;
    app.port(port).multithreaded().run();  // Многопоточный режим
    
    return 0;
}