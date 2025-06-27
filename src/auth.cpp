#include "auth.h"  
#include <openssl/evp.h>  
#include <sstream>
#include <iomanip>
#include <chrono>

AuthService::AuthService(Database& db, const std::string& jwt_secret) 
    : db_(db), jwt_secret_(jwt_secret) {}

std::string AuthService::hash_password(const std::string& password) {
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    const EVP_MD* md = EVP_sha256();
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int len;

    EVP_DigestInit_ex(ctx, md, nullptr);
    EVP_DigestUpdate(ctx, password.c_str(), password.size());
    EVP_DigestFinal_ex(ctx, hash, &len);
    EVP_MD_CTX_free(ctx);

    std::stringstream ss;
    for (unsigned int i = 0; i < len; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

std::string AuthService::generate_token(const std::string& login) {
    auto token = jwt::create()
        .set_issuer("online_chat")
        .set_type("JWS")
        .set_payload_claim("login", jwt::claim(login))
        .set_expires_at(std::chrono::system_clock::now() + std::chrono::hours{24})
        .sign(jwt::algorithm::hs256{jwt_secret_});
    
    return token;
}

bool AuthService::validate_token(const std::string& token) {
    try {
        auto decoded = jwt::decode(token);
        jwt::verify()
            .allow_algorithm(jwt::algorithm::hs256{jwt_secret_})
            .with_issuer("online_chat")
            .verify(decoded);
        return true;
    } catch (...) {
        return false;
    }
}

std::string AuthService::get_login_from_token(const std::string& token) {
    try {
        auto decoded = jwt::decode(token);
        return decoded.get_payload_claim("login").as_string();
    } catch (...) {
        return "";
    }
}

bool AuthService::authenticate(const std::string& login, const std::string& password) {
    User user = db_.get_user_by_login(login);
    if (user.id == 0) return false;
    
    std::string hashed_password = hash_password(password);
    return user.password_hash == hashed_password;
}

bool AuthService::register_user(const std::string& login, const std::string& password) {
    User existing = db_.get_user_by_login(login);
    if (existing.id != 0) return false;
    
    std::string hashed_password = hash_password(password);
    return db_.create_user(login, hashed_password);
}