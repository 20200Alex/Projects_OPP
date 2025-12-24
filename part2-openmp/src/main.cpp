#include "book_analyzer.hpp"
#include <iostream>
#include <vector>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <book_file.txt> [threads]" << std::endl;
        std::cout << "Example: " << argv[0] << " karamazov.txt 4" << std::endl;
        return 1;
    }
    
    std::string filename = argv[1];
    int threads = (argc > 2) ? std::stoi(argv[2]) : 0;
    
    BookAnalyzer analyzer;
    
    try {
        std::cout << "Analyzing file: " << filename << std::endl;
        std::cout << "Using " << (threads == 0 ? "auto-detected" : std::to_string(threads)) 
                  << " threads" << std::endl;
        
        // Анализ с указанным количеством потоков
        auto result = analyzer.analyzeFile(filename, threads);
        
        // Вывод результатов
        BookAnalyzer::printResults(result, 20);
        
        // Сохранение частот в CSV
        BookAnalyzer::saveFrequencyCSV(result, "letter_frequencies.csv");
        
        // Бенчмарк с разным количеством потоков
        std::cout << "\n\n=== BENCHMARKING ===" << std::endl;
        std::vector<int> threadConfigs = {1, 2, 4, 8};
        auto benchmarkResults = analyzer.benchmarkThreads(filename, threadConfigs);
        
        // Вывод результатов бенчмарка
        BookAnalyzer::printBenchmarkResults(benchmarkResults);
        
        // Сохранение результатов бенчмарка
        BookAnalyzer::saveBenchmarkCSV(benchmarkResults, "benchmark_results.csv");
        
        // Генерация графиков
        std::cout << "\n\n=== GENERATING PLOTS ===" << std::endl;
        BookAnalyzer::generatePlotScript(benchmarkResults);
        
        std::cout << "\nAnalysis complete!" << std::endl;
        std::cout << "Generated files:" << std::endl;
        std::cout << "  - letter_frequencies.csv" << std::endl;
        std::cout << "  - benchmark_results.csv" << std::endl;
        std::cout << "  - generate_plots.py" << std::endl;
        std::cout << "  - plot_letter_frequency.py (if data available)" << std::endl;
        
        std::cout << "\nTo generate plots, run: python3 generate_plots.py" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
