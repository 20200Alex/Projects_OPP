cmake_minimum_required(VERSION 3.10)
project(KnightSelectionThreads VERSION 1.0.0 LANGUAGES CXX)

# Настройка стандарта C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Опции для этой части
option(BUILD_TESTS "Build tests for Part 1" ON)
option(BUILD_EXAMPLES "Build examples for Part 1" OFF)

# Настройки компиляции
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -O0 -Wall -Wextra -Wpedantic)
    message(STATUS "Part 1: Debug build enabled")
else()
    add_compile_options(-O2 -Wall)
    message(STATUS "Part 1: Release build enabled")
endif()

# Исходные файлы
set(PART1_SOURCES
    src/KnightSelection.cpp
)

set(PART1_HEADERS
    src/KnightSelection.hpp
)

# Основная библиотека
add_library(KnightSelectionLib ${PART1_SOURCES})

target_include_directories(KnightSelectionLib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}
)

# Основное приложение
add_executable(KnightSelectionApp src/main.cpp)

target_link_libraries(KnightSelectionApp
    KnightSelectionLib
    Threads::Threads  # CMake 3.1+ стиль
)

# Настройка для поиска pthread
find_package(Threads REQUIRED)
if(Threads_FOUND)
    target_link_libraries(KnightSelectionApp Threads::Threads)
    target_link_libraries(KnightSelectionLib Threads::Threads)
endif()

# Тесты
if(BUILD_TESTS)
    enable_testing()
    
    # Поиск GoogleTest
    find_package(GTest QUIET)
    
    if(GTest_FOUND)
        message(STATUS "Part 1: GoogleTest found, building tests")
        
        # Тестовое приложение
        add_executable(knight_selection_tests
            tests/test_knight_selection.cpp
        )
        
        target_link_libraries(knight_selection_tests
            KnightSelectionLib
            GTest::gtest
            GTest::gtest_main
            Threads::Threads
        )
        
        target_include_directories(knight_selection_tests
            PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/src
        )
        
        # Добавление тестов в CTest
        add_test(NAME KnightSelectionTests
            COMMAND knight_selection_tests
        )
        
        # Добавляем цель для запуска тестов
        add_custom_target(run_tests_part1
            COMMAND knight_selection_tests --gtest_color=yes
            DEPENDS knight_selection_tests
            COMMENT "Running Part 1 tests"
        )
        
    else()
        message(WARNING "Part 1: GoogleTest not found, tests will not be built")
        message(STATUS "Install: sudo apt-get install libgtest-dev")
    endif()
endif()

# Примеры (опционально)
if(BUILD_EXAMPLES)
    add_executable(example_simple src/example_simple.cpp)
    target_link_libraries(example_simple KnightSelectionLib)
endif()

# Установка (опционально)
install(TARGETS KnightSelectionApp
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION bin
)

install(TARGETS KnightSelectionLib
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(FILES ${PART1_HEADERS}
    DESTINATION include/part1-threads
)

# Информация о сборке
message(STATUS "Part 1 Configuration:")
message(STATUS "  Project: KnightSelectionThreads")
message(STATUS "  Tests: ${BUILD_TESTS}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
