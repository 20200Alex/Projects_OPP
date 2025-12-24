#include "CityCapture.hpp"
#include <iostream>
#include <mpi.h>

int main(int argc, char* argv[]) {
    // Инициализация MPI
    MPI_Init(&argc, &argv);
    
    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    
    try {
        // Определяем количество городов (по умолчанию 20)
        int num_cities = 20;
        
        if (argc > 1) {
            num_cities = std::atoi(argv[1]);
            if (num_cities < 2 || num_cities > 50) {
                if (world_rank == 0) {
                    std::cerr << "Invalid number of cities. Using default: 20" << std::endl;
                }
                num_cities = 20;
            }
        }
        
        // Проверяем количество процессов
        if (world_rank == 0 && world_size != num_cities + 1) {
            std::cout << "Warning: Running with " << world_size 
                      << " processes, but " << (num_cities + 1)
                      << " recommended (cities + commander)" << std::endl;
            std::cout << "Some processes may be idle." << std::endl;
        }
        
        // Создаем симулятор
        CityCapture simulator(num_cities);
        
        // Запускаем симуляцию
        simulator.simulateCapture();
        
        // Выводим результаты
        if (world_rank == 0) {
            simulator.printResults();
            
            // Проверяем корректность
            bool valid = simulator.validateResults();
            
            std::cout << "\n=== Validation ===" << std::endl;
            if (valid) {
                std::cout << "✓ SUCCESS: All cities have complete cipher!" << std::endl;
                std::cout << "The resistance army has achieved full victory!" << std::endl;
            } else {
                std::cout << "✗ FAILURE: Not all cities have complete cipher." << std::endl;
                std::cout << "The cipher transmission failed!" << std::endl;
            }
        }
        
    } catch (const std::exception& e) {
        if (world_rank == 0) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    
    // Финализация MPI
    MPI_Finalize();
    
    return 0;
}
