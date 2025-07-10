#pragma once
#include <crow.h>
#include "database.h"
#include "auth.h"

// Основной класс веб-приложения чата
class ChatApp {
public:
    // Инициализация с подключением к базе данных и сервису аутентификации
    ChatApp(crow::SimpleApp& app, Database& db, AuthService& auth);
    
    // Проверка авторизации пользователя по запросу
    bool check_auth(const crow::request& req, std::string& login);
    
    // Настройка маршрутов API
    void setup_routes();
    
    // Обработчики HTTP-запросов:
    crow::response handle_chat(const crow::request& req);          // Чат
    crow::response handle_login(const crow::request& req);         // Вход
    crow::response handle_register(const crow::request& req);      // Регистрация
    crow::response handle_get_messages(const crow::request& req);  // Получение сообщений

private:
    crow::SimpleApp& app_;  // Веб-сервер
    Database& db_;          // Доступ к данным
    AuthService& auth_;     // Сервис аутентификации
};