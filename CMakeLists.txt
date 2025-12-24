cmake_minimum_required(VERSION 3.14)
project(KnightSelectionProject VERSION 1.0.0)

# Настройка стандарта C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Опции для сборки
option(BUILD_TESTS "Build tests" ON)
option(BUILD_EXAMPLES "Build examples" ON)

# Основная библиотека
add_library(KnightSelectionLib src/KnightSelection.cpp)
target_include_directories(KnightSelectionLib PUBLIC src)

# Основное приложение
add_executable(KnightSelectionApp src/main.cpp)
target_link_libraries(KnightSelectionApp KnightSelectionLib)

# Тесты
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()

# Установка
install(TARGETS KnightSelectionApp DESTINATION bin)
install(TARGETS KnightSelectionLib DESTINATION lib)
install(FILES src/KnightSelection.hpp DESTINATION include)

# Пакетирование
include(CPack)
