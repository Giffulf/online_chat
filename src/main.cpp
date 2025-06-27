#include "chat.h"
#include "../include/utils.h"
#include <iostream>

int main() {
    // Load environment variables
    auto env_vars = parse_env_file(".env");
    std::string jwt_secret = get_env_var("JWT_SECRET", env_vars);
    std::string db_file = get_env_var("DB_FILE", env_vars);
    int port = std::stoi(get_env_var("PORT", env_vars, "8080")); // Используем значение по умолчанию
    
    if (jwt_secret.empty() || db_file.empty()) {
        std::cerr << "Error: Required environment variables not set" << std::endl;
        return 1;
    }
    
    // Initialize database
    Database db(db_file);
    if (!db.initialize()) {
        std::cerr << "Failed to initialize database" << std::endl;
        return 1;
    }
    
    // Initialize auth service
    AuthService auth(db, jwt_secret);
    
    // Create Crow application
    crow::SimpleApp app;
    
    // Initialize chat application
    ChatApp chat_app(app, db, auth);
    chat_app.setup_routes();
    
    // Start server
    std::cout << "Server running on port " << port << std::endl;
    app.port(port).multithreaded().run();
    
    return 0;
}