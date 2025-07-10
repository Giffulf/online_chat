#pragma once
#include <sqlite3.h>
#include <string>
#include <vector>

// Структура данных пользователя
struct User {
    int id;
    std::string login;
    std::string password_hash;  // Хэш пароля (никогда не храним пароли в чистом виде)
};

// Структура сообщения в чате
struct Message {
    int id;
    int user_id;            // ID отправителя
    std::string user_login; // Логин отправителя (для удобства)
    std::string text;       // Текст сообщения
    std::string timestamp;  // Время отправки
};

// Класс для работы с базой данных SQLite
class Database {
public:
    Database(const std::string& db_file);  // Конструктор с указанием файла БД
    ~Database();                          // Деструктор (закрывает соединение)

    bool initialize();  // Инициализация БД (создание таблиц)
    
    // Операции с пользователями
    bool create_user(const std::string& login, const std::string& password_hash);
    User get_user_by_login(const std::string& login);
    
    // Операции с сообщениями
    bool add_message(int user_id, const std::string& text);
    std::vector<Message> get_recent_messages(int limit = 50);  // Получение последних сообщений

private:
    sqlite3* db_;         // Указатель на соединение с БД
    std::string db_file_;  // Путь к файлу БД
    
    bool execute(const std::string& sql);  // Вспомогательный метод для выполнения SQL
};