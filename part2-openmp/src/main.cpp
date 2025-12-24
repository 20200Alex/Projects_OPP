#include "integral_calculator.hpp"
#include <iostream>
#include <vector>
#include <string>

int main(int argc, char* argv[]) {
    std::cout << "OpenMP Project: Numerical Integration" << std::endl;
    std::cout << "Method: Rectangle Rule with OpenMP" << std::endl;
    
    // Параметры по умолчанию
    size_t segments = 1000000;  // 1 миллион сегментов (меньше для быстрого теста)
    std::vector<int> threadConfigs = {1, 2, 4};
    
    // Парсинг аргументов командной строки
    if (argc > 1) {
        segments = std::stoul(argv[1]);
    }
    if (argc > 2) {
        threadConfigs.clear();
        for (int i = 2; i < argc; ++i) {
            threadConfigs.push_back(std::stoi(argv[i]));
        }
    }
    
    // Создаем калькулятор
    IntegralCalculator calculator;
    
    // Получаем тестовые функции (10 тестов как требуется)
    auto testFunctions = IntegralCalculator::createTestFunctions();
    
    std::cout << "\nConfiguration:" << std::endl;
    std::cout << "  Number of test functions: " << testFunctions.size() << std::endl;
    std::cout << "  Segments per integral: " << segments << std::endl;
    std::cout << "  Thread configurations: ";
    for (int t : threadConfigs) std::cout << t << " ";
    std::cout << std::endl;
    
    // УБРАТЬ вызов omp_get_max_threads() если вызывает проблемы
    // или добавить проверку
    #ifdef _OPENMP
    std::cout << "\nMaximum available threads: " << omp_get_max_threads() << std::endl;
    #else
    std::cout << "\nOpenMP not available" << std::endl;
    #endif
    
    
    try {
        // Запускаем тесты
        auto results = calculator.runTests(testFunctions, segments, threadConfigs);
        
        // Сохраняем результаты
        calculator.saveResultsToCSV(results, "integration_results.csv");
        
        // Генерируем скрипт для построения графиков
        calculator.generatePlotScript(results, "plot_results.py");
        
        // УБРАТЬ автоматический запуск Python скрипта
        std::cout << "\nTo generate plots, run: python3 plot_results.py" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "\nError: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "Program completed successfully!" << std::endl;
    std::cout << "Check integration_results.csv" << std::endl;
    
    return 0;
}
