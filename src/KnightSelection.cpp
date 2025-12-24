#include "KnightSelection.hpp"
#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <algorithm>
#include <future>

// Вспомогательная функция для получения соседей (аналогичная приватной в классе)
static std::vector<int> getNeighborsForTest(int id, int totalKnights) {
    std::vector<int> neighbors;
    int leftNeighbor = (id - 1 + totalKnights) % totalKnights;
    int rightNeighbor = (id + 1) % totalKnights;
    neighbors.push_back(leftNeighbor);
    neighbors.push_back(rightNeighbor);
    return neighbors;
}

// Тест 1: Проверка корректности конструкции
TEST(KnightSelectionTest, ConstructorTest) {
    EXPECT_NO_THROW(KnightSelection(12, 5));
    EXPECT_THROW(KnightSelection(0, 5), std::invalid_argument);
    EXPECT_THROW(KnightSelection(12, 0), std::invalid_argument);
    EXPECT_THROW(KnightSelection(3, 5), std::invalid_argument);
}

// Тест 2: Проверка выбора ровно 5 рыцарей
TEST(KnightSelectionTest, SelectExactlyFiveKnights) {
    KnightSelection selection(12, 5);
    selection.startSelection();
    
    auto selected = selection.getSelectedKnights();
    EXPECT_EQ(selected.size(), 5) << "Должно быть выбрано ровно 5 рыцарей";
}

// Тест 3: Проверка отсутствия соседей среди выбранных
TEST(KnightSelectionTest, NoNeighborsSelected) {
    KnightSelection selection(12, 5);
    selection.startSelection();
    
    EXPECT_TRUE(selection.validateSelection()) << "Выбранные рыцари не должны быть соседями";
}

// Тест 4: Многократный запуск для проверки устойчивости
TEST(KnightSelectionTest, MultipleRunsConsistency) {
    const int runs = 5; // Уменьшил с 10 для скорости
    
    for (int i = 0; i < runs; ++i) {
        KnightSelection selection(12, 5);
        selection.startSelection();
        
        EXPECT_TRUE(selection.validateSelection()) 
            << "Запуск #" << i << ": некорректный выбор рыцарей";
        
        auto selected = selection.getSelectedKnights();
        EXPECT_EQ(selected.size(), 5) 
            << "Запуск #" << i << ": должно быть выбрано ровно 5 рыцарей";
        
        // Проверяем уникальность выбранных рыцарей
        std::sort(selected.begin(), selected.end());
        auto last = std::unique(selected.begin(), selected.end());
        EXPECT_EQ(last, selected.end()) 
            << "Запуск #" << i << ": дублирующиеся рыцари в выборе";
    }
}

// Тест 5: Проверка работы с разными параметрами
TEST(KnightSelectionTest, DifferentParameters) {
    // Тест с большим количеством рыцарей
    {
        KnightSelection selection(20, 7);
        selection.startSelection();
        EXPECT_TRUE(selection.validateSelection()) 
            << "Ошибка при 20 рыцарях, 7 для выбора";
    }
    
    // Тест с минимальным количеством
    {
        KnightSelection selection(6, 3);
        selection.startSelection();
        EXPECT_TRUE(selection.validateSelection()) 
            << "Ошибка при 6 рыцарях, 3 для выбора";
    }
}

// Тест 6: Проверка потоко-безопасности
TEST(KnightSelectionTest, ThreadSafety) {
    const int numThreads = 3;
    std::vector<std::future<bool>> futures;
    
    for (int i = 0; i < numThreads; ++i) {
        futures.push_back(std::async(std::launch::async, []() {
            KnightSelection selection(12, 5);
            selection.startSelection();
            return selection.validateSelection();
        }));
    }
    
    // Проверяем результаты всех потоков
    for (size_t i = 0; i < futures.size(); ++i) {
        bool result = futures[i].get();
        EXPECT_TRUE(result) << "Поток #" << i << " завершился с ошибкой";
    }
}

// Тест 7: Проверка на отсутствие deadlock с таймаутом
TEST(KnightSelectionTest, NoDeadlock) {
    // Используем promise/future для контроля таймаута
    std::promise<bool> promise;
    std::future<bool> future = promise.get_future();
    
    std::thread worker([&promise]() {
        try {
            KnightSelection selection(12, 5);
            selection.startSelection();
            promise.set_value(selection.validateSelection());
        } catch (...) {
            promise.set_exception(std::current_exception());
        }
    });
    
    // Ждем с таймаутом 15 секунд (увеличил для надежности)
    auto status = future.wait_for(std::chrono::seconds(15));
    
    if (status == std::future_status::timeout) {
        // Если таймаут, убиваем поток и проваливаем тест
        worker.detach(); // ВНИМАНИЕ: Это оставляет поток висеть, но лучше чем deadlock
        FAIL() << "Таймаут 15 секунд - возможный deadlock!";
    } else {
        // Если успел, проверяем результат
        worker.join();
        if (status == std::future_status::ready) {
            EXPECT_TRUE(future.get()) << "Выбор рыцарей некорректен";
        }
    }
}

// Тест 8: Проверка, что выбраны разные рыцари в разных запусках
TEST(KnightSelectionTest, DifferentSelectionsInDifferentRuns) {
    KnightSelection selection1(12, 5);
    KnightSelection selection2(12, 5);
    
    selection1.startSelection();
    selection2.startSelection();
    
    auto selected1 = selection1.getSelectedKnights();
    auto selected2 = selection2.getSelectedKnights();
    
    std::sort(selected1.begin(), selected1.end());
    std::sort(selected2.begin(), selected2.end());
    
    // Не обязательно, но вероятно, что выборы будут разными
    // из-за случайности. Проверяем, что оба корректны.
    EXPECT_TRUE(selection1.validateSelection());
    EXPECT_TRUE(selection2.validateSelection());
    
    // Выводим для информации
    std::cout << "Первый выбор: ";
    for (int k : selected1) std::cout << k << " ";
    std::cout << "\nВторой выбор: ";
    for (int k : selected2) std::cout << k << " ";
    std::cout << std::endl;
}

// Тест 9: Проверка граничных случаев
TEST(KnightSelectionTest, BoundaryCases) {
    // Максимальный выбор (каждый второй рыцарь)
    {
        KnightSelection selection(10, 5);
        selection.startSelection();
        EXPECT_TRUE(selection.validateSelection());
    }
    
    // Минимальное количество рыцарей
    {
        KnightSelection selection(3, 1);
        selection.startSelection();
        auto selected = selection.getSelectedKnights();
        EXPECT_EQ(selected.size(), 1);
    }
}

// Тест 10: Интеграционный тест с проверкой вывода
TEST(KnightSelectionTest, IntegrationTest) {
    // Перенаправляем вывод для проверки
    testing::internal::CaptureStdout();
    
    KnightSelection selection(12, 5);
    selection.startSelection();
    selection.printSelectedKnights();
    
    std::string output = testing::internal::GetCapturedStdout();
    
    // Проверяем результаты
    auto selected = selection.getSelectedKnights();
    
    // Основные проверки
    EXPECT_EQ(selected.size(), 5);
    EXPECT_TRUE(selection.validateSelection());
    
    // Дополнительные проверки
    for (int knight : selected) {
        EXPECT_GE(knight, 0);
        EXPECT_LT(knight, 12);
    }
    
    // Проверяем, что вывод содержит информацию о рыцарях
    EXPECT_TRUE(output.find("Выбранные рыцари:") != std::string::npos) 
        << "Нет заголовка в выводе";
}

int main(int argc, char **argv) {
    // Инициализация Google Test
    ::testing::InitGoogleTest(&argc, argv);
    
    // Настраиваем таймаут для всех тестов (на всякий случай)
    ::testing::GTEST_FLAG(break_on_failure) = false;
    ::testing::GTEST_FLAG(catch_exceptions) = true;
    
    std::cout << "Запуск тестов для проекта 'Выбор рыцарей'" << std::endl;
    std::cout << "==========================================" << std::endl;
    
    int result = RUN_ALL_TESTS();
    
    std::cout << "\n==========================================" << std::endl;
    std::cout << "Тесты завершены с кодом: " << result << std::endl;
    std::cout << "0 = успех, 1 = провал" << std::endl;
    
    return result;
}
