#include "database.h"
#include <algorithm>
#include <iostream>
#include <ctime>

Database::Database(const std::string& db_file) : db_file_(db_file), db_(nullptr) {}

Database::~Database() {
    if (db_) {
        sqlite3_close(db_);
    }
}

bool Database::initialize() {
    if (sqlite3_open(db_file_.c_str(), &db_) != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }

    // Create users table
    const char* users_sql = "CREATE TABLE IF NOT EXISTS users ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                           "login TEXT UNIQUE NOT NULL,"
                           "password_hash TEXT NOT NULL);";
    
    // Create messages table
    const char* messages_sql = "CREATE TABLE IF NOT EXISTS messages ("
                              "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                              "user_id INTEGER NOT NULL,"
                              "text TEXT NOT NULL,"
                              "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,"
                              "FOREIGN KEY(user_id) REFERENCES users(id));";

    return execute(users_sql) && execute(messages_sql);
}

bool Database::execute(const std::string& sql) {
    char* err_msg = nullptr;
    if (sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &err_msg) != SQLITE_OK) {
        std::cerr << "SQL error: " << err_msg << std::endl;
        sqlite3_free(err_msg);
        return false;
    }
    return true;
}

bool Database::create_user(const std::string& login, const std::string& password_hash) {
    std::string sql = "INSERT INTO users (login, password_hash) VALUES ('" + 
                      login + "', '" + password_hash + "');";
    return execute(sql);
}

User Database::get_user_by_login(const std::string& login) {
    std::string sql = "SELECT id, login, password_hash FROM users WHERE login = '" + login + "';";
    sqlite3_stmt* stmt;
    User user = {0, "", ""};

    if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            user.id = sqlite3_column_int(stmt, 0);
            user.login = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            user.password_hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        }
        sqlite3_finalize(stmt);
    }
    return user;
}

bool Database::add_message(int user_id, const std::string& text) {
    std::string sql = "INSERT INTO messages (user_id, text) VALUES (" + 
                      std::to_string(user_id) + ", '" + text + "');";
    return execute(sql);
}

std::vector<Message> Database::get_recent_messages(int limit) {
    std::vector<Message> messages;
    std::string sql = "SELECT m.id, m.user_id, u.login, m.text, m.timestamp "
                      "FROM messages m JOIN users u ON m.user_id = u.id "
                      "ORDER BY m.timestamp DESC LIMIT " + std::to_string(limit) + ";";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Message msg;
            msg.id = sqlite3_column_int(stmt, 0);
            msg.user_id = sqlite3_column_int(stmt, 1);
            msg.user_login = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            msg.text = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            msg.timestamp = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            messages.push_back(msg);
        }
        sqlite3_finalize(stmt);
    }
    
    // Reverse to get chronological order
    std::reverse(messages.begin(), messages.end());
    return messages;
}