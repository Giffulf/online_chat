#include "../include/database.h"
#include <cassert>
#include <iostream>

void test_database() {
    std::cout << "Running Database tests...\n";
    
    Database db(":memory:");
    assert(db.initialize());
    
    // Test user management
    assert(db.create_user("user1", "hash1"));
    User u = db.get_user_by_login("user1");
    assert(u.id != 0);
    assert(u.login == "user1");
    assert(u.password_hash == "hash1");
    
    // Test messages
    assert(db.add_message(u.id, "Hello"));
    auto messages = db.get_recent_messages();
    assert(messages.size() == 1);
    assert(messages[0].text == "Hello");
    
    std::cout << "Database tests passed!\n";
}

int main() {
    test_database();
    return 0;
}