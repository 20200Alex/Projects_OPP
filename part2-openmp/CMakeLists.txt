cmake_minimum_required(VERSION 3.10)
project(BookAnalysisOpenMP VERSION 1.0.0 LANGUAGES CXX)

find_package(OpenMP REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_TESTS "Build tests" ON)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -O0 -Wall -Wextra -Wno-multichar)
else()
    add_compile_options(-O3 -Wall)
endif()

if(OpenMP_CXX_FOUND)
    message(STATUS "OpenMP found, version: ${OpenMP_CXX_VERSION}")
    add_compile_options(${OpenMP_CXX_FLAGS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
else()
    message(FATAL_ERROR "OpenMP not found")
endif()

add_executable(book_analysis
    src/main.cpp
    src/book_analyzer.cpp
)

target_include_directories(book_analysis
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(book_analysis OpenMP::OpenMP_CXX)

if(BUILD_TESTS)
    enable_testing()
    
    find_package(GTest QUIET)
    
    if(GTest_FOUND)
        add_executable(book_analysis_tests
            tests/test_book_analyzer.cpp
            src/book_analyzer.cpp
        )
        
        target_include_directories(book_analysis_tests
            PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}/src
        )
        
        target_link_libraries(book_analysis_tests
            GTest::gtest
            GTest::gtest_main
            OpenMP::OpenMP_CXX
        )
        
        add_test(NAME BookAnalysisTests
            COMMAND book_analysis_tests
        )
    endif()
endif()

message(STATUS "Book Analysis project configured successfully")
