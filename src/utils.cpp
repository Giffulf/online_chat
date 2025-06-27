#include "../include/utils.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

std::map<std::string, std::string> parse_env_file(const std::string& filename) {
    std::map<std::string, std::string> env_vars;
    std::ifstream file(filename);
    std::string line;
    
    while (std::getline(file, line)) {
        // Remove comments
        size_t comment_pos = line.find('#');
        if (comment_pos != std::string::npos) {
            line = line.substr(0, comment_pos);
        }
        
        // Trim whitespace
        line.erase(std::remove_if(line.begin(), line.end(), [](unsigned char c) { 
            return std::isspace(c); 
        }), line.end());
        
        if (line.empty()) continue;
        
        size_t equal_pos = line.find('=');
        if (equal_pos != std::string::npos) {
            std::string key = line.substr(0, equal_pos);
            std::string value = line.substr(equal_pos + 1);
            env_vars[key] = value;
        }
    }
    
    return env_vars;
}

std::string get_env_var(const std::string& key, 
                       const std::map<std::string, std::string>& env_vars,
                       const std::string& default_value) {
    auto it = env_vars.find(key);
    if (it != env_vars.end()) {
        return it->second;
    }
    return default_value;
}