#ifndef BOOK_ANALYZER_HPP
#define BOOK_ANALYZER_HPP

#include <string>
#include <vector>
#include <map>
#include <utility>
#include <chrono>
#include <omp.h>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <sstream>

class BookAnalyzer {
public:
    struct AnalysisResult {
        std::map<std::string, int> letterFrequency;      // UTF-8 буква -> частота
        std::vector<std::pair<std::string, int>> sortedLetters; // Отсортированные по убыванию
        std::chrono::microseconds processingTime;        // Время обработки
        int threadsUsed;                                 // Использовано потоков
        int totalLetters;                                // Всего русских букв
        int totalCharacters;                             // Всего символов в тексте
        double speedup;                                  // Ускорение относительно 1 потока
        std::vector<double> speedupHistory;             // История ускорений для графиков
        std::vector<int> threadHistory;                 // История потоков для графиков
    };
    
    BookAnalyzer();
    
    // Основные методы
    AnalysisResult analyzeFile(const std::string& filename, int threads = 0);
    AnalysisResult analyzeText(const std::string& text, int threads = 0);
    
    // Производительность
    std::vector<AnalysisResult> benchmarkThreads(
        const std::string& filename,
        const std::vector<int>& threadConfigs = {1, 2, 4, 8, 16}
    );
    
    // Сохранение результатов
    static void saveFrequencyCSV(const AnalysisResult& result, const std::string& filename);
    static void saveBenchmarkCSV(const std::vector<AnalysisResult>& results, const std::string& filename);
    static void generatePlotScript(const std::vector<AnalysisResult>& benchmarkResults);
    static void generateLetterFrequencyPlot(const AnalysisResult& result);
    
    // Вывод результатов
    static void printResults(const AnalysisResult& result, int topN = 33);
    static void printBenchmarkResults(const std::vector<AnalysisResult>& results);
    
    // Утилиты
    static bool isRussianLetterUTF8(const unsigned char* bytes, size_t& pos, size_t length);
    static std::string getRussianLetterUTF8(const unsigned char* bytes, size_t pos);
    static std::string toLowerRussianUTF8(const std::string& letter);
    static std::string readFileToString(const std::string& filename);

private:
    AnalysisResult analyzeTextImpl(const std::string& text, int threads);
    static std::vector<std::pair<std::string, int>> sortByFrequency(const std::map<std::string, int>& freq);
    static void writePythonPlotScript(const std::string& filename, const std::string& content);
};

#endif
