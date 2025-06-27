#include "../include/database.h"
#include <cassert>

void test_database() {
    Database db(":memory:");
    assert(db.initialize());
    
    // Test user creation
    assert(db.create_user("user1", "hash1"));
    assert(db.create_user("user2", "hash2"));
    
    // Test duplicate user
    assert(!db.create_user("user1", "hash3"));
    
    // Test get user
    auto user1 = db.get_user_by_login("user1");
    assert(user1.id != 0);
    assert(user1.login == "user1");
    assert(user1.password_hash == "hash1");
    
    auto user3 = db.get_user_by_login("nonexistent");
    assert(user3.id == 0);
    
    // Test messages
    assert(db.add_message(user1.id, "Hello world"));
    auto messages = db.get_recent_messages();
    assert(messages.size() == 1);
    assert(messages[0].user_id == user1.id);
    assert(messages[0].text == "Hello world");
    
    std::cout << "All database tests passed!" << std::endl;
}

int main() {
    test_database();
    return 0;
}