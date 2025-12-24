#include "integral_calculator.hpp"
#include <iostream>
#include <fstream>
#include <cmath>
#include <omp.h>
#include <iomanip>
#include <sstream>

IntegralCalculator::IntegralCalculator(int defaultThreads)
    : defaultThreads_(defaultThreads) {
    if (defaultThreads_ <= 0) {
        defaultThreads_ = omp_get_max_threads();
    }
}

IntegralCalculator::Result IntegralCalculator::compute(
    const std::function<double(double)>& func,
    double a, double b,
    size_t segments,
    int threads,
    bool useParallel,
    const std::string& scheduleType,
    int chunkSize
) {
    return computeImpl(func, a, b, segments, threads, useParallel, scheduleType, chunkSize);
}

IntegralCalculator::Result IntegralCalculator::computeImpl(
    const std::function<double(double)>& func,
    double a, double b,
    size_t segments,
    int threads,
    bool useParallel,
    const std::string& scheduleType,
    int chunkSize
) {
    if (threads <= 0) {
        threads = defaultThreads_;
    }
    
    // Устанавливаем число потоков
    omp_set_num_threads(threads);
    
    // Настраиваем schedule
    std::string scheduleClause = "schedule(static)";
    if (scheduleType == "dynamic" && chunkSize > 0) {
        scheduleClause = "schedule(dynamic," + std::to_string(chunkSize) + ")";
    } else if (scheduleType == "dynamic") {
        scheduleClause = "schedule(dynamic)";
    } else if (scheduleType == "guided" && chunkSize > 0) {
        scheduleClause = "schedule(guided," + std::to_string(chunkSize) + ")";
    } else if (scheduleType == "guided") {
        scheduleClause = "schedule(guided)";
    } else if (scheduleType == "runtime") {
        scheduleClause = "schedule(runtime)";
    }
    
    double h = (b - a) / segments;  // Шаг
    double sum = 0.0;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    if (useParallel) {
        #pragma omp parallel
        {
            double localSum = 0.0;
            
            #pragma omp for nowait
            for (size_t i = 0; i < segments; ++i) {
                double x = a + (i + 0.5) * h;  // Средняя точка прямоугольника
                localSum += func(x);
            }
            
            #pragma omp atomic
            sum += localSum;
        }
    } else {
        // Однопоточная версия для сравнения
        for (size_t i = 0; i < segments; ++i) {
            double x = a + (i + 0.5) * h;
            sum += func(x);
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double integral = sum * h;
    
    return Result{
        integral,
        0.0,  // Погрешность будет вычислена позже
        duration,
        threads,
        segments,
        1.0   // Ускорение будет вычислено позже
    };
}

std::vector<IntegralCalculator::Result> IntegralCalculator::runTests(
    const std::vector<TestFunction>& testFunctions,
    size_t segmentsPerTest,
    const std::vector<int>& threadConfigs
) {
    std::vector<Result> allResults;
    
    std::cout << "==========================================" << std::endl;
    std::cout << "Running OpenMP Integral Tests" << std::endl;
    std::cout << "Segments per test: " << segmentsPerTest << std::endl;
    std::cout << "Thread configurations: ";
    for (int t : threadConfigs) std::cout << t << " ";
    std::cout << std::endl;
    std::cout << "==========================================" << std::endl;
    
    // Сначала получаем однопоточные результаты для вычисления ускорения
    std::vector<double> singleThreadTimes(testFunctions.size());
    
    for (size_t testIdx = 0; testIdx < testFunctions.size(); ++testIdx) {
        const auto& test = testFunctions[testIdx];
        
        std::cout << "\nTest " << (testIdx + 1) << "/" << testFunctions.size() 
                  << ": " << test.name << std::endl;
        std::cout << "  Interval: [" << test.a << ", " << test.b << "]" << std::endl;
        std::cout << "  Expected: " << test.expected << std::endl;
        
        // Запускаем для всех конфигураций потоков
        for (int threads : threadConfigs) {
            Result result = compute(
                test.func, test.a, test.b,
                segmentsPerTest, threads, true, "static"
            );
            
            // Вычисляем погрешность относительно аналитического значения
            result.error = std::abs(result.value - test.expected);
            
            // Сохраняем время однопоточного выполнения для вычисления ускорения
            if (threads == 1) {
                singleThreadTimes[testIdx] = result.time.count() / 1000000.0; // в секундах
            }
            
            // Добавляем информацию о тесте
            result.speedup = (threads == 1) ? 1.0 : 
                singleThreadTimes[testIdx] / (result.time.count() / 1000000.0);
            
            allResults.push_back(result);
            
            std::cout << "  Threads: " << std::setw(2) << threads 
                      << " | Result: " << std::setw(12) << std::setprecision(8) << result.value
                      << " | Error: " << std::setw(10) << std::scientific << result.error
                      << " | Time: " << std::setw(8) << result.time.count() / 1000.0 << " ms"
                      << " | Speedup: " << std::setw(6) << std::fixed << std::setprecision(2) << result.speedup
                      << std::endl;
        }
    }
    
    return allResults;
}

void IntegralCalculator::saveResultsToCSV(
    const std::vector<Result>& results,
    const std::string& filename
) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }
    
    // Заголовок CSV
    file << "test_index,function_name,threads,segments,result,error,time_ms,speedup,expected\n";
    
    for (size_t i = 0; i < results.size(); ++i) {
        const auto& r = results[i];
        file << i << ","
             << "test_function" << ","  // Можно добавить имена функций
             << r.threads << ","
             << r.segments << ","
             << std::setprecision(12) << r.value << ","
             << std::scientific << r.error << ","
             << r.time.count() / 1000.0 << ","
             << std::fixed << std::setprecision(3) << r.speedup << ","
             << "0.0\n";  // Ожидаемое значение можно добавить
    }
    
    file.close();
    std::cout << "\nResults saved to: " << filename << std::endl;
}

std::vector<IntegralCalculator::TestFunction> IntegralCalculator::createTestFunctions() {
    std::vector<TestFunction> tests;
    
    // 1. Константная функция
    tests.push_back({
        "f(x) = 1",
        [](double x) { return 1.0; },
        0.0, 1.0,
        1.0  // ∫₀¹ 1 dx = 1
    });
    
    // 2. Линейная функция
    tests.push_back({
        "f(x) = x",
        [](double x) { return x; },
        0.0, 1.0,
        0.5  // ∫₀¹ x dx = 0.5
    });
    
    // 3. Квадратичная функция
    tests.push_back({
        "f(x) = x^2",
        [](double x) { return x * x; },
        0.0, 1.0,
        1.0/3.0  // ∫₀¹ x² dx = 1/3
    });
    
    // 4. Кубическая функция
    tests.push_back({
        "f(x) = x^3",
        [](double x) { return x * x * x; },
        0.0, 1.0,
        0.25  // ∫₀¹ x³ dx = 1/4
    });
    
    // 5. Синус
    tests.push_back({
        "f(x) = sin(x)",
        [](double x) { return std::sin(x); },
        0.0, M_PI,
        2.0  // ∫₀^π sin(x) dx = 2
    });
    
    // 6. Косинус
    tests.push_back({
        "f(x) = cos(x)",
        [](double x) { return std::cos(x); },
        0.0, M_PI/2,
        1.0  // ∫₀^{π/2} cos(x) dx = 1
    });
    
    // 7. Экспонента
    tests.push_back({
        "f(x) = e^x",
        [](double x) { return std::exp(x); },
        0.0, 1.0,
        M_E - 1.0  // ∫₀¹ e^x dx = e - 1
    });
    
    // 8. Логарифм
    tests.push_back({
        "f(x) = ln(x+1)",
        [](double x) { return std::log(x + 1.0); },
        0.0, M_E - 1.0,
        1.0  // ∫₀^{e-1} ln(x+1) dx = 1
    });
    
    // 9. Сложная тригонометрическая
    tests.push_back({
        "f(x) = sin(x) * cos(x)",
        [](double x) { return std::sin(x) * std::cos(x); },
        0.0, M_PI/2,
        0.5  // ∫₀^{π/2} sin(x)cos(x) dx = 1/2
    });
    
    // 10. Полином 4-й степени
    tests.push_back({
        "f(x) = x^4 - 2x^2 + 1",
        [](double x) { return x*x*x*x - 2*x*x + 1; },
        -1.0, 1.0,
        1.6  // ∫₋₁¹ (x⁴ - 2x² + 1) dx = 1.6
    });
    
    return tests;
}

void IntegralCalculator::generatePlotScript(
    const std::vector<Result>& results,
    const std::string& filename
) {
    std::ofstream script(filename);
    if (!script.is_open()) {
        std::cerr << "Error: Could not create plot script" << std::endl;
        return;
    }
    
    script << R"(#!/usr/bin/env python3
import matplotlib.pyplot as plt
import numpy as np
import csv

# Чтение данных
results = []
with open('results.csv', 'r') as f:
    reader = csv.DictReader(f)
    for row in reader:
        results.append(row)

# Преобразование данных
threads = sorted(set(int(r['threads']) for r in results))
speedups = {t: [] for t in threads}

for r in results:
    t = int(r['threads'])
    if t > 0:
        speedups[t].append(float(r['speedup']))

# Среднее ускорение для каждого числа потоков
avg_speedup = [np.mean(speedups[t]) for t in threads]
std_speedup = [np.std(speedups[t]) for t in threads]

# Линейное ускорение (идеальное)
linear_speedup = threads

# Построение графиков
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 5))

# График 1: Зависимость ускорения от числа потоков
ax1.plot(threads, avg_speedup, 'bo-', label='Фактическое ускорение', linewidth=2)
ax1.plot(threads, linear_speedup, 'r--', label='Линейное ускорение', linewidth=2)
ax1.fill_between(threads, 
                 [a-s for a,s in zip(avg_speedup, std_speedup)],
                 [a+s for a,s in zip(avg_speedup, std_speedup)],
                 alpha=0.2)
ax1.set_xlabel('Число потоков')
ax1.set_ylabel('Ускорение')
ax1.set_title('Зависимость ускорения от числа потоков (OpenMP)')
ax1.grid(True, alpha=0.3)
ax1.legend()

# График 2: Эффективность
efficiency = [avg_speedup[i] / threads[i] for i in range(len(threads))]
ax2.plot(threads, efficiency, 'go-', linewidth=2)
ax2.axhline(y=1.0, color='r', linestyle='--', alpha=0.5)
ax2.set_xlabel('Число потоков')
ax2.set_ylabel('Эффективность')
ax2.set_title('Эффективность параллелизации')
ax2.grid(True, alpha=0.3)
ax2.set_ylim([0, 1.1])

plt.tight_layout()
plt.savefig('speedup_plot.png', dpi=150)
plt.savefig('speedup_plot.pdf')
print("Графики сохранены как speedup_plot.png и speedup_plot.pdf")

# Дополнительный анализ
print("\n=== Анализ результатов ===")
for i, t in enumerate(threads):
    print(f"Потоков: {t:2d} | Ускорение: {avg_speedup[i]:.2f} ± {std_speedup[i]:.2f} | "
          f"Эффективность: {efficiency[i]:.1%}")
)";
    
    script.close();
    std::cout << "Python script generated: " << filename << std::endl;
    
    // Делаем скрипт исполняемым (для Linux)
    #ifdef __linux__
    std::string command = "chmod +x " + filename;
    system(command.c_str());
    #endif
}
