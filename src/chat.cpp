#include "chat.h"
#include <fstream>
#include <sstream>
#include <vector>

ChatApp::ChatApp(crow::SimpleApp& app, Database& db, AuthService& auth) 
    : app_(app), db_(db), auth_(auth) {}

bool ChatApp::check_auth(const crow::request& req, std::string& login) {
    auto auth_header = req.get_header_value("Authorization");
    if (auth_header.empty()) {
        auto cookie = req.get_header_value("Cookie");
        size_t token_pos = cookie.find("auth_token=");
        if (token_pos == std::string::npos) return false;
        
        size_t token_start = token_pos + 11;
        size_t token_end = cookie.find(";", token_start);
        std::string token = cookie.substr(token_start, token_end - token_start);
        
        if (!auth_.validate_token(token)) return false;
        login = auth_.get_login_from_token(token);
        return true;
    }
    
    if (auth_header.find("Bearer ") != 0) return false;
    std::string token = auth_header.substr(7);
    if (!auth_.validate_token(token)) return false;
    login = auth_.get_login_from_token(token);
    return true;
}

void ChatApp::setup_routes() {
    // Serve static HTML files
    CROW_ROUTE(app_, "/")([](const crow::request& req) {
        std::ifstream file("static/index.html");
        if (!file.is_open()) {
            return crow::response(404, "Not Found");
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        auto response = crow::response(buffer.str());
        response.add_header("Content-Type", "text/html");
        return response;
    });
    
    CROW_ROUTE(app_, "/login")([this](const crow::request& req) {
        std::ifstream file("static/login.html");
        if (!file.is_open()) {
            return crow::response(404, "Not Found");
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        auto response = crow::response(buffer.str());
        response.add_header("Content-Type", "text/html");
        return response;
    });
    CROW_ROUTE(app_, "/script.js")([]{
        std::ifstream file("static/script.js");
        if (!file.is_open()) {
            file.open("../static/script.js"); 
        }   
        if (!file.is_open()) {
            return crow::response(404);
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        auto res = crow::response(buffer.str());
        res.set_header("Content-Type", "application/javascript");
        return res;
    });
    CROW_ROUTE(app_, "/favicon.ico")([]{
        auto res = crow::response(204); // No Content
        res.set_header("Content-Type", "image/x-icon");
        return res;
    });
   


    // API routes
    CROW_ROUTE(app_, "/api/chat").methods("POST"_method)([this](const crow::request& req) {
        return handle_chat(req);
    });
    
    CROW_ROUTE(app_, "/api/login").methods("POST"_method)([this](const crow::request& req) {
    try {
        // Проверяем Content-Type
        if (req.get_header_value("Content-Type") != "application/json") {
            return crow::response(400, "{\"error\":\"Требуется Content-Type: application/json\"}");
        }

        // Парсим JSON
        auto body = crow::json::load(req.body);
        if (!body || !body.has("login") || !body.has("password")) {
            return crow::response(400, "{\"error\":\"Требуется логин и пароль\"}");
        }

        std::string login = body["login"].s();
        std::string password = body["password"].s();

        if (!auth_.authenticate(login, password)) {
            return crow::response(401, "{\"error\":\"Неверный логин или пароль\"}");
        }

        std::string token = auth_.generate_token(login);
        auto response = crow::response(200, "{\"status\":\"success\"}");
        response.add_header("Content-Type", "application/json");
        response.add_header("Set-Cookie", 
            "auth_token=" + token + 
            "; Path=/; Max-Age=86400; HttpOnly; SameSite=Lax");
        return response;

    } catch (const std::exception& e) {
        return crow::response(500, "{\"error\":\"Ошибка сервера\"}");
    }
});

    
    CROW_ROUTE(app_, "/api/register").methods("POST"_method)([this](const crow::request& req) {
    try {
        auto body = crow::json::load(req.body);
        if (!body || !body.has("login") || !body.has("password")) {
            return crow::response(400, 
                crow::json::wvalue{{"error", "Invalid request format"}}.dump());
        }

        std::string login = body["login"].s();
        std::string password = body["password"].s();

        if (!auth_.register_user(login, password)) {
            return crow::response(400, 
                crow::json::wvalue{{"error", "Registration failed"}}.dump());
        }

        return crow::response(201, 
            crow::json::wvalue{{"status", "success"}}.dump());
    } catch (const std::exception& e) {
        return crow::response(500, 
            crow::json::wvalue{{"error", "Internal server error"}}.dump());
    }
});

    
    CROW_ROUTE(app_, "/api/messages").methods("GET"_method)([this](const crow::request& req) {
        return handle_get_messages(req);
    });
}

crow::response ChatApp::handle_chat(const crow::request& req) {
    std::string login;
    if (!check_auth(req, login)) {
        return crow::response(401, "Unauthorized");
    }
    
    auto body = crow::json::load(req.body);
    if (!body || !body.has("message")) {
        return crow::response(400, "Bad Request");
    }
    
    User user = db_.get_user_by_login(login);
    if (user.id == 0) {
        return crow::response(401, "Unauthorized");
    }
    
    if (!db_.add_message(user.id, body["message"].s())) {
        return crow::response(500, "Internal Server Error");
    }
    
    return crow::response(200, "Message sent");
}

crow::response ChatApp::handle_login(const crow::request& req) {
    auto body = crow::json::load(req.body);
    if (!body || !body.has("login") || !body.has("password")) {
        return crow::response(400, "Bad Request");
    }
    
    std::string login = body["login"].s();
    std::string password = body["password"].s();
    
    if (!auth_.authenticate(login, password)) {
        return crow::response(401, "Invalid credentials");
    }
    
    std::string token = auth_.generate_token(login);
    auto response = crow::response(200, R"({"status":"success"})");
    response.add_header("Content-Type", "application/json");
    response.add_header("Set-Cookie", 
        "auth_token=" + token + 
        "; Path=/" + 
        "; Max-Age=86400" + 
        "; HttpOnly");
    
    return response;
}
crow::response ChatApp::handle_register(const crow::request& req) {
    auto body = crow::json::load(req.body);
    if (!body || !body.has("login") || !body.has("password")) {
        return crow::response(400, "Bad Request");
    }
    
    std::string login = body["login"].s();
    std::string password = body["password"].s();
    
    if (!auth_.register_user(login, password)) {
        return crow::response(400, "Registration failed");
    }
    
    return crow::response(201, "User created");
}

crow::response ChatApp::handle_get_messages(const crow::request& req) {
    std::string login;
    if (!check_auth(req, login)) {
        return crow::response(401, "Unauthorized");
    }
    
    auto messages = db_.get_recent_messages();
    crow::json::wvalue result;
    result["messages"] = crow::json::wvalue::list();
    
    for (size_t i = 0; i < messages.size(); ++i) {
        result["messages"][i]["user"] = messages[i].user_login;
        result["messages"][i]["text"] = messages[i].text;
        result["messages"][i]["time"] = messages[i].timestamp;
    }
    
    auto response = crow::response(result);
    response.add_header("Content-Type", "application/json");
    return response;
}