cmake_minimum_required(VERSION 3.10)
project(OpenMPIntegral VERSION 1.0.0 LANGUAGES CXX)

# Поиск OpenMP
find_package(OpenMP REQUIRED)

# Настройка стандарта C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Опции проекта
option(BUILD_TESTS "Build tests" ON)

# Флаги компиляции
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -O0 -Wall -Wextra)
else()
    add_compile_options(-O3 -Wall -march=native)
endif()

# Добавляем OpenMP флаги
if(OpenMP_CXX_FOUND)
    add_compile_options(${OpenMP_CXX_FLAGS})
    link_libraries(${OpenMP_CXX_FLAGS})
endif()

# Основное приложение
add_executable(integral_app
    src/main.cpp
    src/integral_calculator.cpp
)

target_include_directories(integral_app
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Тесты
if(BUILD_TESTS)
    find_package(GTest)
    
    if(GTest_FOUND)
        add_executable(integral_tests
            tests/test_integral.cpp
            src/integral_calculator.cpp
        )
        
        target_include_directories(integral_tests
            PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/src
        )
        
        target_link_libraries(integral_tests
            GTest::gtest
            GTest::gtest_main
        )
        
        add_test(NAME IntegralTests COMMAND integral_tests)
    endif()
endif()

# Скрипт для запуска тестов производительности
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/scripts/run_benchmark.sh.in
    ${CMAKE_CURRENT_BINARY_DIR}/run_benchmark.sh
    @ONLY
)
