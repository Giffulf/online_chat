#include "../include/utils.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

/**
 * Парсит файл .env и возвращает пары ключ-значение
 * @param filename Путь к файлу .env
 * @return Словарь переменных окружения (ключ → значение)
 * 
 * Обрабатывает файл построчно:
 * - Игнорирует комментарии (после #)
 * - Удаляет пробельные символы
 * - Разбивает строки по символу '='
 */
std::map<std::string, std::string> parse_env_file(const std::string& filename) {
    std::map<std::string, std::string> env_vars;
    std::ifstream file(filename);
    std::string line;
    
    while (std::getline(file, line)) {
        // Удаление комментариев (все что после #)
        size_t comment_pos = line.find('#');
        if (comment_pos != std::string::npos) {
            line = line.substr(0, comment_pos);
        }
        
        // Удаление всех пробельных символов
        line.erase(std::remove_if(line.begin(), line.end(), [](unsigned char c) { 
            return std::isspace(c); 
        }), line.end());
        
        // Пропуск пустых строк
        if (line.empty()) continue;
        
        // Разделение строки на ключ и значение
        size_t equal_pos = line.find('=');
        if (equal_pos != std::string::npos) {
            std::string key = line.substr(0, equal_pos);
            std::string value = line.substr(equal_pos + 1);
            env_vars[key] = value;
        }
    }
    
    return env_vars;
}

/**
 * Получает значение переменной окружения из словаря
 * @param key Имя переменной
 * @param env_vars Словарь переменных окружения
 * @param default_value Значение по умолчанию, если переменная не найдена
 * @return Значение переменной или default_value если не найдена
 * 
 * Пример использования:
 * get_env_var("PORT", env_vars, "8080") → вернет PORT или "8080"
 */
std::string get_env_var(const std::string& key, 
                       const std::map<std::string, std::string>& env_vars,
                       const std::string& default_value) {
    auto it = env_vars.find(key);
    if (it != env_vars.end()) {
        return it->second;
    }
    return default_value;
}