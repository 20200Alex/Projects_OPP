#include "book_analyzer.hpp"
#include <iostream>
#include <vector>
#include <string>

int main(int argc, char* argv[]) {
    std::cout << "OpenMP Project: Analysis of 'Brothers Karamazov'" << std::endl;
    std::cout << "Variant 15: Frequency of Russian letters" << std::endl;
    
    BookAnalyzer analyzer;
    
    // Определяем имя файла
    std::string filename;
    if (argc > 1) {
        filename = argv[1];
    } else {
        // Попробуем найти файл по умолчанию
        std::vector<std::string> possibleFiles = {
            "Достоевский Федор. Том 9. Братья Карамазовы - royallib.ru.txt",
            "karamazov.txt",
            "book.txt",
            "data/karamazov.txt",
            "data/book.txt"
        };
        
        for (const auto& fname : possibleFiles) {
            std::ifstream testFile(fname);
            if (testFile.good()) {
                filename = fname;
                testFile.close();
                break;
            }
        }
        
        if (filename.empty()) {
            std::cout << "\nERROR: No book file found!" << std::endl;
            std::cout << "Please provide the book file as argument:" << std::endl;
            std::cout << "  ./book_analysis <book_file.txt>" << std::endl;
            std::cout << "\nExpected file: 'Достоевский Федор. Том 9. Братья Карамазовы - royallib.ru.txt'" << std::endl;
            return 1;
        }
    }
    
    std::cout << "\nAnalyzing file: " << filename << std::endl;
    
    try {
        // Проверяем файл
        std::ifstream testFile(filename);
        if (!testFile.good()) {
            throw std::runtime_error("Cannot open file: " + filename);
        }
        testFile.close();
        
        // Определяем конфигурации потоков
        std::vector<int> threadConfigs;
        int maxThreads = omp_get_max_threads();
        
        // Автоматически выбираем конфигурации
        if (maxThreads >= 16) {
            threadConfigs = {1, 2, 4, 8, 16};
        } else if (maxThreads >= 8) {
            threadConfigs = {1, 2, 4, 8};
        } else if (maxThreads >= 4) {
            threadConfigs = {1, 2, 4};
        } else {
            threadConfigs = {1, 2};
        }
        
        std::cout << "\nSystem Information:" << std::endl;
        std::cout << "  Max available threads: " << maxThreads << std::endl;
        std::cout << "  Benchmark configurations: ";
        for (int t : threadConfigs) std::cout << t << " ";
        std::cout << std::endl;
        
        // Шаг 1: Бенчмарк производительности
        std::cout << "\n[1/3] Running performance benchmark..." << std::endl;
        auto benchmarkResults = analyzer.benchmarkThreads(filename, threadConfigs);
        
        // Шаг 2: Детальный анализ с оптимальным количеством потоков
        std::cout << "\n[2/3] Detailed analysis with optimal thread count..." << std::endl;
        
        // Находим оптимальное количество потоков
        int optimalThreads = 1;
        double bestSpeedup = 0;
        for (const auto& result : benchmarkResults) {
            if (result.speedup > bestSpeedup) {
                bestSpeedup = result.speedup;
                optimalThreads = result.threadsUsed;
            }
        }
        
        std::cout << "  Using " << optimalThreads << " threads (best speedup: " 
                  << std::fixed << std::setprecision(2) << bestSpeedup << "x)" << std::endl;
        
        auto detailedResult = analyzer.analyzeFile(filename, optimalThreads);
        detailedResult.speedup = bestSpeedup;
        
        // Шаг 3: Сохранение результатов
        std::cout << "\n[3/3] Saving results and generating plots..." << std::endl;
        
        // Сохраняем данные
        analyzer.saveFrequencyCSV(detailedResult, "letter_frequencies.csv");
        analyzer.saveBenchmarkCSV(benchmarkResults, "benchmark_results.csv");
        
        // Генерируем скрипты для графиков
        analyzer.generatePlotScript(benchmarkResults);
        
        // Выводим результаты
        analyzer.printResults(detailedResult);
        analyzer.printBenchmarkResults(benchmarkResults);
        
        std::cout << "ANALYSIS COMPLETE" << std::endl;
        std::cout << "\nGenerated files:" << std::endl;
        std::cout << "  1. letter_frequencies.csv - Frequencies of all Russian letters" << std::endl;
        std::cout << "  2. benchmark_results.csv - Performance data for different thread counts" << std::endl;
        std::cout << "  3. generate_plots.py - Python script to generate all graphs" << std::endl;
        std::cout << "  4. plot_letter_frequency.py - Script for letter frequency plots" << std::endl;
        std::cout << "\nTo generate graphs, run:" << std::endl;
        std::cout << "  python3 generate_plots.py" << std::endl;
        std::cout << "  python3 plot_letter_frequency.py" << std::endl;
        std::cout << "\nOr directly:" << std::endl;
        std::cout << "  ./generate_plots.py" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "\nERROR: " << e.what() << std::endl;
        std::cerr << "\nMake sure the book file exists and is in UTF-8 encoding." << std::endl;
        return 1;
    }
    
    return 0;
}
