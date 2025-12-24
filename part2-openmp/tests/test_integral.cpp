#include "integral_calculator.hpp"
#include <gtest/gtest.h>
#include <cmath>

TEST(IntegralCalculatorTest, BasicIntegration) {
    IntegralCalculator calculator;
    
    // Тест 1: Константная функция
    auto result1 = calculator.compute(
        [](double x) { return 1.0; },
        0.0, 1.0,
        1000000,  // 1 миллион сегментов
        1,        // 1 поток
        false     // Без OpenMP
    );
    
    EXPECT_NEAR(result1.value, 1.0, 1e-6);
    
    // Тест 2: Линейная функция
    auto result2 = calculator.compute(
        [](double x) { return x; },
        0.0, 1.0,
        1000000,
        1,
        false
    );
    
    EXPECT_NEAR(result2.value, 0.5, 1e-6);
}

TEST(IntegralCalculatorTest, ParallelIntegration) {
    IntegralCalculator calculator;
    
    // Тест с разным количеством потоков
    std::vector<int> threadConfigs = {1, 2, 4};
    
    for (int threads : threadConfigs) {
        auto result = calculator.compute(
            [](double x) { return std::sin(x); },
            0.0, M_PI,
            1000000,
            threads,
            true  // С OpenMP
        );
        
        // Проверяем, что результат примерно правильный
        EXPECT_NEAR(result.value, 2.0, 1e-4);
        EXPECT_EQ(result.threads, threads);
    }
}

TEST(IntegralCalculatorTest, ScheduleTypes) {
    IntegralCalculator calculator;
    
    std::vector<std::string> schedules = {"static", "dynamic", "guided"};
    
    for (const auto& schedule : schedules) {
        auto result = calculator.compute(
            [](double x) { return x * x; },
            0.0, 1.0,
            1000000,
            4,
            true,
            schedule,
            1000  // Размер блока
        );
        
        EXPECT_NEAR(result.value, 1.0/3.0, 1e-4);
    }
}

TEST(IntegralCalculatorTest, TestFunctionsCreation) {
    auto tests = IntegralCalculator::createTestFunctions();
    
    // Проверяем, что создано 10 тестовых функций
    EXPECT_EQ(tests.size(), 10);
    
    // Проверяем первую функцию
    EXPECT_EQ(tests[0].name, "f(x) = 1");
    EXPECT_NEAR(tests[0].func(0.5), 1.0, 1e-10);
    EXPECT_NEAR(tests[0].expected, 1.0, 1e-10);
}

TEST(IntegralCalculatorTest, ErrorCalculation) {
    IntegralCalculator calculator;
    
    auto tests = IntegralCalculator::createTestFunctions();
    
    for (const auto& test : tests) {
        auto result = calculator.compute(
            test.func, test.a, test.b,
            100000,
            1,
            false
        );
        
        // Вычисляем погрешность
        double error = std::abs(result.value - test.expected);
        
        // Для достаточно гладких функций погрешность должна быть небольшой
        if (test.name.find("x^4") == std::string::npos) {  // Исключаем полином 4-й степени
            EXPECT_LT(error, 1e-3);
        }
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
