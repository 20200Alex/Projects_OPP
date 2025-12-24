#ifndef BOOK_ANALYZER_HPP
#define BOOK_ANALYZER_HPP

#include <string>
#include <vector>
#include <map>
#include <utility>
#include <chrono>
#include <omp.h>

/**
 * @class BookAnalyzer
 * @brief Анализатор частоты букв в тексте с использованием OpenMP
 */
class BookAnalyzer {
public:
    /**
     * @brief Результат анализа
     */
    struct AnalysisResult {
        std::map<char, int> letterFrequency;      // Частота каждой буквы
        std::vector<std::pair<char, int>> sortedLetters; // Отсортированные по частоте
        int totalLetters;                          // Общее количество букв
        int totalCharacters;                       // Общее количество символов
        std::chrono::microseconds processingTime; // Время обработки
        int threadsUsed;                          // Количество использованных потоков
        double speedup;                           // Ускорение относительно одного потока
    };
    
    /**
     * @brief Конструктор
     */
    BookAnalyzer();
    
    /**
     * @brief Анализирует текст из файла
     * @param filename Путь к файлу
     * @param threads Количество потоков (0 = auto)
     * @param chunkSize Размер блока для динамического распределения
     * @return Результат анализа
     */
    AnalysisResult analyzeFile(const std::string& filename, 
                               int threads = 0, 
                               int chunkSize = 1000);
    
    /**
     * @brief Анализирует текст из строки
     * @param text Текст для анализа
     * @param threads Количество потоков
     * @return Результат анализа
     */
    AnalysisResult analyzeText(const std::string& text, int threads = 0);
    
    /**
     * @brief Запускает тесты производительности
     * @param filename Путь к файлу
     * @param threadConfigs Конфигурации потоков для тестирования
     * @return Вектор результатов
     */
    std::vector<AnalysisResult> runPerformanceTests(
        const std::string& filename,
        const std::vector<int>& threadConfigs = {1, 2, 4, 8}
    );
    
    /**
     * @brief Сохраняет результаты в CSV файл
     * @param results Результаты тестов
     * @param filename Имя файла
     */
    static void saveResultsToCSV(
        const std::vector<AnalysisResult>& results,
        const std::string& filename
    );
    
    /**
     * @brief Генерирует скрипт Python для построения графиков
     * @param results Результаты тестов
     * @param filename Имя файла скрипта
     */
    static void generatePlotScript(
        const std::vector<AnalysisResult>& results,
        const std::string& filename
    );
    
    /**
     * @brief Выводит топ-N букв
     * @param result Результат анализа
     * @param topN Количество букв для вывода
     */
    static void printTopLetters(const AnalysisResult& result, int topN = 10);
    
    /**
     * @brief Создает тестовый текст для проверки
     * @return Тестовый текст
     */
    static std::string createTestText();

private:
    /**
     * @brief Проверяет, является ли символ русской буквой
     * @param c Символ
     * @return true если русская буква
     */
    static bool isRussianLetter(char c);
    
    /**
     * @brief Преобразует символ в нижний регистр для русского алфавита
     * @param c Символ
     * @return Символ в нижнем регистре
     */
    static char toLowerRussian(char c);
    
    /**
     * @brief Сортирует буквы по частоте
     * @param frequency Карта частот
     * @return Вектор пар (буква, частота), отсортированный по убыванию
     */
    static std::vector<std::pair<char, int>> sortByFrequency(
        const std::map<char, int>& frequency
    );
    
    /**
     * @brief Внутренняя реализация анализа текста
     */
    AnalysisResult analyzeTextImpl(const std::string& text, int threads);
};

#endif // BOOK_ANALYZER_HPP
