cmake_minimum_required(VERSION 3.10)
project(OpenMPIntegral VERSION 1.0.0 LANGUAGES CXX)

# Поиск OpenMP
find_package(OpenMP REQUIRED)

# Настройка стандарта C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Опции для этой части
option(BUILD_TESTS "Build tests for Part 2" ON)
option(ENABLE_COVERAGE "Enable code coverage" OFF)
option(GENERATE_PLOTS "Generate plots with Python" OFF)  # Отключаем по умолчанию

# Настройки вывода
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Настройки компиляции
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -O0 -Wall -Wextra)
    message(STATUS "Part 2: Debug build enabled")
    
    if(ENABLE_COVERAGE)
        add_compile_options(--coverage -fprofile-arcs -ftest-coverage)
        add_link_options(--coverage)
        message(STATUS "Part 2: Code coverage enabled")
    endif()
else()
    add_compile_options(-O3 -Wall -march=native)
    message(STATUS "Part 2: Release build with optimizations")
endif()

# Добавляем OpenMP флаги
if(OpenMP_CXX_FOUND)
    message(STATUS "Part 2: OpenMP found")
    add_compile_options(${OpenMP_CXX_FLAGS})
else()
    message(FATAL_ERROR "Part 2: OpenMP not found! Install: sudo apt-get install libomp-dev")
endif()

# Исходные файлы
set(PART2_SOURCES
    src/integral_calculator.cpp
)

set(PART2_HEADERS
    src/integral_calculator.hpp
)

# Основное приложение
add_executable(integral_app
    src/main.cpp
    ${PART2_SOURCES}
)

target_include_directories(integral_app
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}
)

# Связываем OpenMP
target_link_libraries(integral_app OpenMP::OpenMP_CXX)

# Тесты
if(BUILD_TESTS)
    enable_testing()
    
    # Поиск GoogleTest
    find_package(GTest QUIET)
    
    if(GTest_FOUND)
        message(STATUS "Part 2: GoogleTest found, building tests")
        
        # Тестовое приложение
        add_executable(integral_tests
            tests/test_integral.cpp
            ${PART2_SOURCES}
        )
        
        target_link_libraries(integral_tests
            GTest::gtest
            GTest::gtest_main
            OpenMP::OpenMP_CXX
        )
        
        target_include_directories(integral_tests
            PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/src
        )
        
        # Добавление тестов в CTest
        add_test(NAME IntegralTests
            COMMAND integral_tests
        )
        
        # Цель для запуска тестов
        add_custom_target(run_tests_part2
            COMMAND integral_tests --gtest_color=yes
            DEPENDS integral_tests
            COMMENT "Running Part 2 tests"
        )
        
    else()
        message(WARNING "Part 2: GoogleTest not found, tests will not be built")
    endif()
endif()

# Генерация скриптов для графиков
if(GENERATE_PLOTS)
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/scripts)
        # Копируем Python скрипт в билд директорию
        configure_file(
            ${CMAKE_CURRENT_SOURCE_DIR}/scripts/plot_results.py.in
            ${CMAKE_BINARY_DIR}/plot_results.py
            @ONLY
        )
        
        # Создаем цель для генерации графиков
        add_custom_target(generate_plots
            COMMAND ${CMAKE_COMMAND} -E echo "Generating plots..."
            COMMAND cd ${CMAKE_BINARY_DIR} && python3 plot_results.py || echo "Python script executed"
            DEPENDS integral_app
            COMMENT "Generating performance plots"
        )
    else()
        message(STATUS "Part 2: scripts directory not found, plot generation disabled")
        set(GENERATE_PLOTS OFF)
    endif()
endif()

# Скрипт для бенчмарков 
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/scripts/run_benchmark.sh.in)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/scripts/run_benchmark.sh.in
        ${CMAKE_BINARY_DIR}/run_benchmark.sh
        @ONLY
    )
    
    # Делаем скрипт исполняемым
    add_custom_command(
        TARGET integral_app POST_BUILD
        COMMAND chmod +x ${CMAKE_BINARY_DIR}/run_benchmark.sh
        COMMENT "Making benchmark script executable"
    )
else()
    # Создаем простой скрипт для бенчмарков
    file(WRITE ${CMAKE_BINARY_DIR}/run_simple_test.sh
        "#!/bin/bash\n"
        "echo 'Running simple OpenMP test...'\n"
        "./integral_app 1000000 1 2 4\n"
        "echo 'Test completed'\n"
    )
    
    add_custom_command(
        TARGET integral_app POST_BUILD
        COMMAND chmod +x ${CMAKE_BINARY_DIR}/run_simple_test.sh
        COMMENT "Creating simple test script"
    )
endif()

# Установка
install(TARGETS integral_app
    RUNTIME DESTINATION bin
)

install(FILES ${PART2_HEADERS}
    DESTINATION include/part2-openmp
)

# Информация о сборке
message(STATUS "Part 2 Configuration:")
message(STATUS "  Project: OpenMPIntegral")
message(STATUS "  OpenMP: ${OpenMP_CXX_VERSION}")
message(STATUS "  Tests: ${BUILD_TESTS}")
message(STATUS "  Plots: ${GENERATE_PLOTS}")
message(STATUS "  Coverage: ${ENABLE_COVERAGE}")
