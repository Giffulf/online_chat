cmake_minimum_required(VERSION 3.14)
project(online_chat)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Включение FetchContent для загрузки зависимостей
include(FetchContent)

# Загрузка и настройка Crow
FetchContent_Declare(
    crow
    GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
    GIT_TAG        v1.0+5
)

# Загрузка jwt-cpp
FetchContent_Declare(
    jwt-cpp
    GIT_REPOSITORY https://github.com/Thalhammer/jwt-cpp.git
    GIT_TAG        v0.7.0
)

# Загрузка зависимостей
FetchContent_MakeAvailable(crow jwt-cpp)

# Поиск системных зависимостей
find_package(OpenSSL REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(Threads REQUIRED)
find_package(Boost 1.64 REQUIRED COMPONENTS system)

# Проверка наличия Crow
if(NOT TARGET Crow::Crow)
    message(FATAL_ERROR "Crow library not found! Check installation.")
endif()

# Исходные файлы
file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "include/*.h")

# Создание исполняемого файла
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Подключение зависимостей
target_link_libraries(${PROJECT_NAME} PRIVATE
    Crow::Crow
    jwt-cpp::jwt-cpp
    OpenSSL::SSL
    OpenSSL::Crypto
    SQLite::SQLite3
    Threads::Threads
    Boost::system
)

# Настройка путей включения
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Копирование дополнительных файлов
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/static ${CMAKE_CURRENT_BINARY_DIR}/static
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/.env.template ${CMAKE_CURRENT_BINARY_DIR}/.env
)