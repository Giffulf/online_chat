#pragma once
#include <string>
#include <jwt-cpp/jwt.h>
#include "database.h"

// Сервис для аутентификации и работы с JWT токенами
class AuthService {
public:
    AuthService(Database& db, const std::string& jwt_secret);
    
    // Генерация/валидация JWT токенов
    std::string generate_token(const std::string& login);
    bool validate_token(const std::string& token);
    std::string get_login_from_token(const std::string& token);
    
    // Проверка учетных данных и регистрация
    bool authenticate(const std::string& login, const std::string& password);
    bool register_user(const std::string& login, const std::string& password);

private:
    Database& db_;             // Доступ к базе данных
    std::string jwt_secret_;   // Секрет для подписи токенов
    
    std::string hash_password(const std::string& password);
};