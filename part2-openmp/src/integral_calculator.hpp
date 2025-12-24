#ifndef INTEGRAL_CALCULATOR_HPP
#define INTEGRAL_CALCULATOR_HPP

#include <vector>
#include <functional>
#include <string>
#include <chrono>
#include <omp.h>

/**
 * @class IntegralCalculator
 * @brief Класс для вычисления определенного интеграла методом прямоугольников с использованием OpenMP
 */
class IntegralCalculator {
public:
    /**
     * @brief Результат вычисления интеграла
     */
    struct Result {
        double value;                          ///< Значение интеграла
        double error;                          ///< Погрешность (если известна аналитическая формула)
        std::chrono::microseconds time;        ///< Время выполнения в микросекундах
        int threads;                           ///< Количество использованных потоков
        size_t segments;                       ///< Количество сегментов разбиения
        double speedup;                        ///< Ускорение относительно однопоточной версии
    };
    
    /**
     * @brief Тестовая функция для проверки
     */
    struct TestFunction {
        std::string name;                      ///< Название функции
        std::function<double(double)> func;    ///< Сама функция
        double a;                              ///< Нижний предел интегрирования
        double b;                              ///< Верхний предел интегрирования
        double expected;                       ///< Ожидаемое значение интеграла (аналитически)
    };
    
    /**
     * @brief Конструктор
     * @param defaultThreads Число потоков по умолчанию
     */
    explicit IntegralCalculator(int defaultThreads = 0);
    
    /**
     * @brief Вычисляет интеграл методом прямоугольников
     * @param func Подынтегральная функция
     * @param a Нижний предел
     * @param b Верхний предел
     * @param segments Количество сегментов разбиения
     * @param threads Количество потоков (0 = auto)
     * @param useParallel Использовать OpenMP
     * @param scheduleType Тип распределения итераций (static, dynamic, guided)
     * @param chunkSize Размер блока для dynamic/guided
     * @return Результат вычисления
     */
    Result compute(
        const std::function<double(double)>& func,
        double a, double b,
        size_t segments = 1000000,
        int threads = 0,
        bool useParallel = true,
        const std::string& scheduleType = "static",
        int chunkSize = 0
    );
    
    /**
     * @brief Запускает серию тестов
     * @param testFunctions Вектор тестовых функций
     * @param segmentsPerTest Количество сегментов для каждого теста
     * @param threadConfigs Конфигурации потоков для тестирования
     * @return Вектор результатов
     */
    std::vector<Result> runTests(
        const std::vector<TestFunction>& testFunctions,
        size_t segmentsPerTest = 1000000,
        const std::vector<int>& threadConfigs = {1, 2, 4, 8}
    );
    
    /**
     * @brief Сохраняет результаты в CSV файл
     * @param results Результаты тестов
     * @param filename Имя файла
     */
    static void saveResultsToCSV(
        const std::vector<Result>& results,
        const std::string& filename
    );
    
    /**
     * @brief Создает тестовые функции (10 тестов как требуется)
     * @return Вектор тестовых функций
     */
    static std::vector<TestFunction> createTestFunctions();
    
    /**
     * @brief Генерирует скрипт Python для построения графиков
     * @param results Результаты тестов
     * @param filename Имя файла скрипта
     */
    static void generatePlotScript(
        const std::vector<Result>& results,
        const std::string& filename
    );

private:
    int defaultThreads_;
    
    /**
     * @brief Внутренняя реализация вычисления интеграла
     */
    Result computeImpl(
        const std::function<double(double)>& func,
        double a, double b,
        size_t segments,
        int threads,
        bool useParallel,
        const std::string& scheduleType,
        int chunkSize
    );
};

#endif // INTEGRAL_CALCULATOR_HPP
