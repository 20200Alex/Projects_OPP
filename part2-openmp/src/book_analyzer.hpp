#ifndef BOOK_ANALYZER_HPP
#define BOOK_ANALYZER_HPP

#include <string>
#include <vector>
#include <map>
#include <chrono>

class BookAnalyzer {
public:
    // Структура для хранения результатов анализа
    struct AnalysisResult {
        std::map<std::string, int> letterFrequency;
        std::vector<std::pair<std::string, int>> sortedLetters;
        std::chrono::microseconds processingTime;
        int threadsUsed;
        int totalLetters;
        int totalCharacters;
        double speedup;
        std::vector<int> threadHistory;
        std::vector<double> speedupHistory;
    };
    
    BookAnalyzer();
    
    // Основные методы анализа
    AnalysisResult analyzeFile(const std::string& filename, int threads = 0);
    AnalysisResult analyzeText(const std::string& text, int threads = 0);
    
    // Бенчмарк и производительность
    std::vector<AnalysisResult> benchmarkThreads(
        const std::string& filename,
        const std::vector<int>& threadConfigs = {1, 2, 4, 8});
    
    // Сохранение результатов
    static void saveFrequencyCSV(const AnalysisResult& result, const std::string& filename);
    static void saveBenchmarkCSV(
        const std::vector<AnalysisResult>& results,
        const std::string& filename);
    
    // Генерация графиков
    static void generatePlotScript(const std::vector<AnalysisResult>& benchmarkResults);
    static void generateLetterFrequencyPlot(const AnalysisResult& result);
    
    // Вывод результатов
    static void printResults(const AnalysisResult& result, int topN = 20);
    static void printBenchmarkResults(const std::vector<AnalysisResult>& results);
    
    // Статические методы для тестов
    static bool isRussianLetter(char c);
    static char toLowerRussian(char c);
    static std::string createTestText();
    
private:
    // Вспомогательные методы для UTF-8
    static bool isRussianLetterUTF8(const unsigned char* bytes, size_t& pos, size_t length);
    static std::string getRussianLetterUTF8(const unsigned char* bytes, size_t pos);
    static std::string toLowerRussianUTF8(const std::string& letter);
    
    // Вспомогательные методы
    static std::string readFileToString(const std::string& filename);
    static std::vector<std::pair<std::string, int>> sortByFrequency(
        const std::map<std::string, int>& freq);
    static void writePythonPlotScript(const std::string& filename, const std::string& content);
    
    // Основная реализация анализа
    AnalysisResult analyzeTextImpl(const std::string& text, int threads);
};

#endif // BOOK_ANALYZER_HPP
