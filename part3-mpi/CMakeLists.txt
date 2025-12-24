cmake_minimum_required(VERSION 3.14)
project(Part3_MPI_CityCapture VERSION 1.0.0)

# Поиск MPI
find_package(MPI REQUIRED)

# Настройка стандарта C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Опции сборки
option(BUILD_TESTS "Build tests for Part 3" ON)

# Флаги компиляции
add_compile_options(-Wall -Wextra)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -O0)
else()
    add_compile_options(-O3)
endif()

# Основное приложение
add_executable(city_capture_app
    src/main.cpp
    src/CityCapture.cpp
)

target_include_directories(city_capture_app
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(city_capture_app
    MPI::MPI_CXX
)

# Установка
install(TARGETS city_capture_app
    RUNTIME DESTINATION bin
)

# Тесты
if(BUILD_TESTS)
    enable_testing()
    
    find_package(GTest QUIET)
    
    if(GTest_FOUND)
        add_executable(city_capture_tests
            tests/test_city_capture.cpp
            src/CityCapture.cpp
        )
        
        target_include_directories(city_capture_tests
            PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/src
        )
        
        target_link_libraries(city_capture_tests
            GTest::gtest
            GTest::gtest_main
            MPI::MPI_CXX
        )
        
        add_test(NAME CityCaptureTests
            COMMAND ${MPIEXEC} ${MPIEXEC_NUMPROC_FLAG} 4 ${MPIEXEC_PREFLAGS} 
                    ./city_capture_tests ${MPIEXEC_POSTFLAGS}
        )
        
    else()
        message(WARNING "GoogleTest not found, tests will not be built")
    endif()
endif()

# Информация о сборке
message(STATUS "Part 3 Configuration:")
message(STATUS "  Project: City Capture (MPI)")
message(STATUS "  MPI found: YES")
message(STATUS "  Tests: ${BUILD_TESTS}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
