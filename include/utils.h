#pragma once
#include <string>
#include <map>

std::map<std::string, std::string> parse_env_file(const std::string& filename);
std::string get_env_var(const std::string& key, 
                       const std::map<std::string, std::string>& env_vars,
                       const std::string& default_value = "");
                       