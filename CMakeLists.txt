cmake_minimum_required(VERSION 3.14)
project(KnightSelectionProject VERSION 1.0.0 LANGUAGES CXX)

# Настройка стандарта C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Опции проекта
option(BUILD_TESTS "Build tests with Google Test" ON)
option(BUILD_EXAMPLES "Build example programs" OFF)

# Настройки компиляции
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -O0 -Wall -Wextra -Wpedantic)
else()
    add_compile_options(-O2 -Wall)
endif()

# Поиск pthread
find_package(Threads REQUIRED)

# Основная библиотека проекта
add_library(KnightSelectionLib
    src/KnightSelection.cpp
)

target_include_directories(KnightSelectionLib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Основное приложение
add_executable(KnightSelectionApp
    src/main.cpp
)

target_link_libraries(KnightSelectionApp
    KnightSelectionLib
    Threads::Threads
)

# Тестирование
if(BUILD_TESTS)
    enable_testing()
    
    # Поиск GoogleTest
    find_package(GTest)
    
    if(GTest_FOUND)
        message(STATUS "Google Test found, building tests")
        
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
        
        # Создаем каталог для исполняемого файла тестов
        set_target_properties(knight_selection_tests PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
        )
    else()
        message(WARNING "Google Test not found, tests will not be built")
    endif()
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
