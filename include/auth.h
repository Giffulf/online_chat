#pragma once
#include <string>
#include <jwt-cpp/jwt.h>
#include "database.h"

class AuthService {
public:
    AuthService(Database& db, const std::string& jwt_secret);
    
    std::string generate_token(const std::string& login);
    bool validate_token(const std::string& token);
    std::string get_login_from_token(const std::string& token);
    
    bool authenticate(const std::string& login, const std::string& password);
    bool register_user(const std::string& login, const std::string& password);

private:
    Database& db_;
    std::string jwt_secret_;
    
    std::string hash_password(const std::string& password);
};
