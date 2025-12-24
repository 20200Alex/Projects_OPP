#ifndef BOOK_ANALYZER_HPP
#define BOOK_ANALYZER_HPP

#include <string>
#include <vector>
#include <map>
#include <utility>
#include <chrono>
#include <omp.h>

class BookAnalyzer {
public:
    struct AnalysisResult {
        std::map<char, int> letterFrequency;
        std::vector<std::pair<char, int>> sortedLetters;
        int totalLetters;
        int totalCharacters;
        std::chrono::microseconds processingTime;
        int threadsUsed;
        double speedup;
    };
    
    BookAnalyzer();
    
    AnalysisResult analyzeFile(const std::string& filename, 
                               int threads = 0, 
                               int chunkSize = 1000);
    
    AnalysisResult analyzeText(const std::string& text, int threads = 0);
    
    std::vector<AnalysisResult> runPerformanceTests(
        const std::string& filename,
        const std::vector<int>& threadConfigs = {1, 2, 4, 8}
    );
    
    static void saveResultsToCSV(
        const std::vector<AnalysisResult>& results,
        const std::string& filename
    );
    
    static void generatePlotScript(
        const std::vector<AnalysisResult>& results,
        const std::string& filename
    );
    
    static void printTopLetters(const AnalysisResult& result, int topN = 10);
    
    static std::string createTestText();

    // Делаем методы публичными для тестов
    static bool isRussianLetter(unsigned char c);
    static char toLowerRussian(unsigned char c);

private:
    static std::vector<std::pair<char, int>> sortByFrequency(
        const std::map<char, int>& frequency
    );
    
    AnalysisResult analyzeTextImpl(const std::string& text, int threads);
};

#endif
