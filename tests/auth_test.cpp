#include "../include/auth.h"
#include "../include/database.h"
#include <cassert>
#include <iostream>

void test_auth_service() {
    Database db(":memory:");
    assert(db.initialize());
    AuthService auth(db, "test_secret");
    
    // Тестируем только публичные методы
    // 1. Тест регистрации и аутентификации
    assert(auth.register_user("test_user", "test_pass"));
    assert(auth.authenticate("test_user", "test_pass"));
    assert(!auth.authenticate("test_user", "wrong_pass"));
    
    // 2. Тест JWT токена
    std::string token = auth.generate_token("test_user");
    assert(auth.validate_token(token));
    assert(auth.get_login_from_token(token) == "test_user");
    
    std::cout << "AuthService tests passed!\n";
}

int main() {
    test_auth_service();
    return 0;
}