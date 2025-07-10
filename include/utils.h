#pragma once
#include <string>
#include <map>

/**
 * Парсит файл .env и возвращает пары ключ-значение
 * @param filename Путь к файлу .env
 * @return Словарь переменных окружения
 */
std::map<std::string, std::string> parse_env_file(const std::string& filename);

/**
 * Получает значение переменной окружения из словаря
 * @param key Имя переменной
 * @param env_vars Словарь переменных окружения
 * @param default_value Значение по умолчанию, если переменная не найдена
 * @return Значение переменной или default_value, если не найдена
 */
std::string get_env_var(const std::string& key,
                       const std::map<std::string, std::string>& env_vars,
                       const std::string& default_value = "");