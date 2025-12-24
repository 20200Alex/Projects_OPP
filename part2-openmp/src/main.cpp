#include "book_analyzer.hpp"
#include <iostream>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    std::cout << "==========================================" << std::endl;
    std::cout << "    OpenMP Russian Text Analyzer" << std::endl;
    std::cout << "    Book: Brothers Karamazov" << std::endl;
    std::cout << "    Author: Fyodor Dostoevsky" << std::endl;
    std::cout << "==========================================" << std::endl;
    
    // Путь к файлу по умолчанию
    std::string filename;
    if (argc > 1) {
        filename = argv[1];
    } else {
        // Пробуем найти файл в разных местах
        std::vector<std::string> possiblePaths = {
            "data/karamazov.txt",
            "../data/karamazov.txt",
            "karamazov.txt",
            "./karamazov.txt"
        };
        
        for (const auto& path : possiblePaths) {
            if (fs::exists(path)) {
                filename = path;
                std::cout << "Found book file: " << filename << std::endl;
                break;
            }
        }
        
        if (filename.empty()) {
            std::cout << "\n❌ ERROR: Book file not found!" << std::endl;
            std::cout << "Please provide the path to 'karamazov.txt'" << std::endl;
            std::cout << "\nUsage: " << argv[0] << " <book_file.txt> [threads]" << std::endl;
            std::cout << "Example: " << argv[0] << " data/karamazov.txt 4" << std::endl;
            return 1;
        }
    }
    
    int threads = (argc > 2) ? std::stoi(argv[2]) : 0;
    
    BookAnalyzer analyzer;
    
    try {
        std::cout << "\n📖 Analyzing file: " << filename << std::endl;
        std::cout << "⚡ Using " << (threads == 0 ? "auto-detected" : std::to_string(threads)) 
                  << " threads" << std::endl;
        
        // 1. Анализ с указанным количеством потоков
        std::cout << "\n🔍 Performing initial analysis..." << std::endl;
        auto result = analyzer.analyzeFile(filename, threads);
        
        // Вывод результатов
        BookAnalyzer::printResults(result, 20);
        
        // Сохранение частот в CSV
        BookAnalyzer::saveFrequencyCSV(result, "letter_frequencies.csv");
        
        // 2. Бенчмарк с разным количеством потоков
        std::cout << "\n\n🏃‍♂️ Starting performance benchmark..." << std::endl;
        std::vector<int> threadConfigs = {1, 2, 4, 8};
        auto benchmarkResults = analyzer.benchmarkThreads(filename, threadConfigs);
        
        // Вывод результатов бенчмарка
        BookAnalyzer::printBenchmarkResults(benchmarkResults);
        
        // Сохранение результатов бенчмарка
        BookAnalyzer::saveBenchmarkCSV(benchmarkResults, "benchmark_results.csv");
        
        // 3. Генерация графиков
        std::cout << "\n\n🎨 Generating performance plots..." << std::endl;
        BookAnalyzer::generatePlotScript(benchmarkResults);
        BookAnalyzer::generateLetterFrequencyPlot(result);
        
        std::cout << "\n✅ Analysis complete!" << std::endl;
        std::cout << "\n📁 Generated files:" << std::endl;
        std::cout << "   1. 📊 letter_frequencies.csv" << std::endl;
        std::cout << "   2. ⚡ benchmark_results.csv" << std::endl;
        std::cout << "   3. 🖼️  openmp_performance_analysis.png" << std::endl;
        std::cout << "   4. 🖼️  speedup_comparison.png" << std::endl;
        std::cout << "   5. 🖼️  letter_frequency_analysis.png" << std::endl;
        std::cout << "   6. 📜 generate_plots.py" << std::endl;
        std::cout << "   7. 📜 plot_speedup.py" << std::endl;
        std::cout << "   8. 📜 plot_letter_frequency.py" << std::endl;
        
        std::cout << "\n💡 To generate plots manually, run:" << std::endl;
        std::cout << "   $ python3 generate_plots.py" << std::endl;
        std::cout << "   $ python3 plot_speedup.py" << std::endl;
        std::cout << "   $ python3 plot_letter_frequency.py" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "\n❌ Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
