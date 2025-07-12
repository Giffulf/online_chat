# Тестирование приложения Online Chat

## Структура тестов

```
online_chat/
├── tests/
│   ├── auth_test.cpp       # Тесты сервиса аутентификации
│   ├── database_test.cpp   # Тесты работы с базой данных
```

## Зависимости для тестирования

- CMake 3.14+
- SQLite3
- OpenSSL
- C++17 компилятор

## Сборка и запуск тестов

### Сборка тестов

```bash
cd online_chat/
cmake -B build -DBUILD_TESTING=ON
cmake --build build --target run_all_tests
```

### Запуск отдельных тестов

```bash
# Только тесты аутентификации
./tests/auth_test

# Только тесты базы данных
./tests/database_test
```

## Описание тестов

### Тесты аутентификации (`auth_test.cpp`)

**Проверяемые функции:**
- Регистрация пользователей
- Аутентификация
- Работа с JWT токенами
- Хеширование паролей

**Пример теста:**
```cpp
TEST_CASE("User registration") {
    Database db(":memory:");
    AuthService auth(db, "secret");
    
    REQUIRE(auth.register_user("test", "password"));
    REQUIRE_FALSE(auth.register_user("test", "password")); // Дубликат
}
```

### Тесты базы данных (`database_test.cpp`)

**Проверяемые функции:**
- Создание таблиц
- Работа с пользователями
- Сохранение и получение сообщений

**Пример теста:**
```cpp
TEST_CASE("Message storage") {
    Database db(":memory:");
    db.initialize();
    
    db.create_user("user1", "hash1");
    auto user = db.get_user_by_login("user1");
    
    REQUIRE(db.add_message(user.id, "Hello"));
    auto messages = db.get_recent_messages();
    REQUIRE(messages[0].text == "Hello");
}
```