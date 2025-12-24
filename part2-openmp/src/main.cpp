#include "book_analyzer.hpp"
#include <iostream>
#include <vector>
#include <string>

int main(int argc, char* argv[]) {
    std::cout << "OpenMP Project: Book Analysis" << std::endl;
    std::cout << "Variant 15: Frequency of Russian letters" << std::endl;
    
    BookAnalyzer analyzer;
    
    // Путь к файлу (можно передать как аргумент)
    std::string filename = "data/karamazov_sample.txt";
    if (argc > 1) {
        filename = argv[1];
    }
    
    // Конфигурации потоков для тестирования
    std::vector<int> threadConfigs = {1, 2, 4, 8};
    if (argc > 2) {
        threadConfigs.clear();
        for (int i = 2; i < argc; ++i) {
            threadConfigs.push_back(std::stoi(argv[i]));
        }
    }
    
    std::cout << "\nConfiguration:" << std::endl;
    std::cout << "  Input file: " << filename << std::endl;
    std::cout << "  Thread configurations: ";
    for (int t : threadConfigs) std::cout << t << " ";
    std::cout << std::endl;
    
    #ifdef _OPENMP
    std::cout << "\nMaximum available threads: " << omp_get_max_threads() << std::endl;
    #endif
    
    
    try {
        // Запускаем тесты производительности
        auto results = analyzer.runPerformanceTests(filename, threadConfigs);
        
        if (!results.empty()) {
            // Сохраняем результаты
            analyzer.saveResultsToCSV(results, "book_analysis_results.csv");
            
            // Генерируем скрипт для графиков
            analyzer.generatePlotScript(results, "plot_book_analysis.py");
            
            // Выводим детальный анализ для последнего результата
            const auto& lastResult = results.back();
            analyzer.printTopLetters(lastResult, 15);
            
            std::cout << "Analysis completed successfully!" << std::endl;
            std::cout << "Files generated:" << std::endl;
            std::cout << "  - book_analysis_results.csv" << std::endl;
            std::cout << "  - plot_book_analysis.py" << std::endl;
            std::cout << "  - book_analysis_speedup.png" << std::endl;
        } else {
            std::cout << "\nNo results were generated." << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "\nError: " << e.what() << std::endl;
        
        // Если не удалось прочитать файл, используем тестовый текст
        std::cout << "\nUsing test text for demonstration..." << std::endl;
        
        std::string testText = BookAnalyzer::createTestText();
        auto result = analyzer.analyzeText(testText, 1);
        
        analyzer.printTopLetters(result, 10);
        
        std::cout << "\nNote: For full analysis, provide a text file as argument" << std::endl;
        std::cout << "Usage: " << argv[0] << " <filename> [threads...]" << std::endl;
        
        return 1;
    }
    
    return 0;
}
