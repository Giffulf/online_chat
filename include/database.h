#pragma once
#include <sqlite3.h>
#include <string>
#include <vector>

struct User {
    int id;
    std::string login;
    std::string password_hash;
};

struct Message {
    int id;
    int user_id;
    std::string user_login;
    std::string text;
    std::string timestamp;
};

class Database {
public:
    Database(const std::string& db_file);
    ~Database();

    bool initialize();
    
    // User operations
    bool create_user(const std::string& login, const std::string& password_hash);
    User get_user_by_login(const std::string& login);
    
    // Message operations
    bool add_message(int user_id, const std::string& text);
    std::vector<Message> get_recent_messages(int limit = 50);

private:
    sqlite3* db_;
    std::string db_file_;
    
    bool execute(const std::string& sql);
};
