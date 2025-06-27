#pragma once
#include <crow.h>
#include "database.h"
#include "auth.h"

class ChatApp {
public:
    ChatApp(crow::SimpleApp& app, Database& db, AuthService& auth);
    
    bool check_auth(const crow::request& req, std::string& login);
    void setup_routes();
    
    crow::response handle_chat(const crow::request& req);
    crow::response handle_login(const crow::request& req);
    crow::response handle_register(const crow::request& req);
    crow::response handle_get_messages(const crow::request& req);

private:
    crow::SimpleApp& app_;
    Database& db_;
    AuthService& auth_;
};