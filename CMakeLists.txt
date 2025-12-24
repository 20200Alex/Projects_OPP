cmake_minimum_required(VERSION 3.14)
project(ParallelProgrammingProject VERSION 1.0.0)

# Описание проекта
message(STATUS "Parallel Programming Project")
message(STATUS "Part 1: Threads - Knight Selection")
message(STATUS "Part 2: OpenMP - Numerical Integration")
message(STATUS "Part 3: MPI - (to be implemented)")

# Основные опции
option(BUILD_ALL_PARTS "Build all project parts" ON)
option(BUILD_PART1 "Build Part 1 (Threads)" ON)
option(BUILD_PART2 "Build Part 2 (OpenMP)" ON)

# Если нужно собирать все из корня
if(BUILD_ALL_PARTS)
    if(BUILD_PART1)
        message(STATUS "Adding Part 1: Threads")
        add_subdirectory(part1-threads)
    endif()
    
    if(BUILD_PART2)
        message(STATUS "Adding Part 2: OpenMP")
        add_subdirectory(part2-openmp)
    endif()
else()
    message(STATUS "Note: Use individual CMakeLists.txt in each part directory")
    message(STATUS "  cd part1-threads && mkdir build && cd build && cmake ..")
    message(STATUS "  cd part2-openmp && mkdir build && cd build && cmake ..")
endif()

# Установочные цели (опционально)
install(DIRECTORY part1-threads/src/ DESTINATION include/part1-threads)
install(DIRECTORY part2-openmp/src/ DESTINATION include/part2-openmp)
