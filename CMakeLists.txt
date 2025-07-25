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

   add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
       COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/static
       COMMAND ${CMAKE_COMMAND} -E copy
           ${CMAKE_CURRENT_SOURCE_DIR}/static/script.js
           ${CMAKE_CURRENT_BINARY_DIR}/static/script.js
       COMMAND ${CMAKE_COMMAND} -E copy
           ${CMAKE_CURRENT_SOURCE_DIR}/static/index.html
           ${CMAKE_CURRENT_BINARY_DIR}/static/index.html
   )
   


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

# Тесты
if(BUILD_TESTING)
    enable_testing()
    
    # Тест базы данных
    add_executable(online_chat_db_test tests/database_test.cpp src/database.cpp)
    target_include_directories(online_chat_db_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_link_libraries(online_chat_db_test PRIVATE SQLite::SQLite3)
    add_test(NAME db_test COMMAND online_chat_db_test)

    # Тест аутентификации
    add_executable(online_chat_auth_test tests/auth_test.cpp src/auth.cpp src/database.cpp)
    target_include_directories(online_chat_auth_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_link_libraries(online_chat_auth_test PRIVATE 
        SQLite::SQLite3 
        OpenSSL::SSL 
        OpenSSL::Crypto
        jwt-cpp::jwt-cpp
    )
    add_test(NAME auth_test COMMAND online_chat_auth_test)

    # Мета-цель для запуска всех тестов
    add_custom_target(run_all_tests
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        DEPENDS online_chat_db_test online_chat_auth_test
    )
endif()