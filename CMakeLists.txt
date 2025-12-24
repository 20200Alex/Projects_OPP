cmake_minimum_required(VERSION 3.14)
project(KnightSelectionProject VERSION 1.0.0 LANGUAGES CXX)

# Настройка стандарта C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Опции проекта
option(BUILD_TESTS "Build tests with Google Test" ON)
option(BUILD_EXAMPLES "Build example programs" OFF)
option(ENABLE_COVERAGE "Enable code coverage reporting" OFF)

# Настройки компиляции
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -O0 -Wall -Wextra -Wpedantic -Werror)
else()
    add_compile_options(-O2 -Wall)
endif()

# Включение покрытия кода
if(ENABLE_COVERAGE)
    add_compile_options(--coverage -fprofile-arcs -ftest-coverage)
    add_link_options(--coverage)
endif()

# Основная библиотека проекта
add_library(KnightSelectionLib STATIC
    src/KnightSelection.cpp
)

target_include_directories(KnightSelectionLib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
)

# Основное приложение
add_executable(KnightSelectionApp
    src/main.cpp
)

target_link_libraries(KnightSelectionApp
    PRIVATE
        KnightSelectionLib
        pthread
)

# Тестирование
if(BUILD_TESTS)
    enable_testing()
    
    # Поиск GoogleTest
    find_package(GTest REQUIRED)
    
    # Тестовое приложение
    add_executable(knight_selection_tests
        tests/test_knight_selection.cpp
    )
    
    target_link_libraries(knight_selection_tests
        PRIVATE
            KnightSelectionLib
            GTest::gtest
            GTest::gtest_main
            pthread
    )
    
    target_include_directories(knight_selection_tests
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
    )
    
    # Добавление тестов в CTest
    add_test(NAME KnightSelectionTests
        COMMAND knight_selection_tests
    )
endif()

# Установка
install(TARGETS KnightSelectionApp
    RUNTIME DESTINATION bin
)

install(TARGETS KnightSelectionLib
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(FILES src/KnightSelection.hpp
    DESTINATION include
)

# Пакетирование
include(CPack)
