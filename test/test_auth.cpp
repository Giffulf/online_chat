#include "../include/auth.h"
#include "../include/database.h"
#include <cassert>

void test_auth() {
    Database db(":memory:");
    db.initialize();
    
    AuthService auth(db, "test_secret");
    
    // Test registration
    assert(auth.register_user("test_user", "password123"));
    assert(!auth.register_user("test_user", "password123")); // Should fail - user exists
    
    // Test authentication
    assert(auth.authenticate("test_user", "password123"));
    assert(!auth.authenticate("test_user", "wrong_password"));
    assert(!auth.authenticate("nonexistent_user", "password"));
    
    // Test token generation and validation
    std::string token = auth.generate_token("test_user");
    assert(auth.validate_token(token));
    assert(auth.get_login_from_token(token) == "test_user");
    
    // Test invalid token
    assert(!auth.validate_token("invalid.token.here"));
    assert(auth.get_login_from_token("invalid.token.here").empty());
    
    std::cout << "All auth tests passed!" << std::endl;
}

int main() {
    test_auth();
    return 0;
}