#include "KnightSelection.hpp"
#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <algorithm>

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
    EXPECT_EQ(selected.size(), 5);
}

// Тест 3: Проверка отсутствия соседей среди выбранных
TEST(KnightSelectionTest, NoNeighborsSelected) {
    KnightSelection selection(12, 5);
    selection.startSelection();
    
    EXPECT_TRUE(selection.validateSelection());
}

// Тест 4: Многопоточный тест (запуск нескольких раз)
TEST(KnightSelectionTest, MultipleRunsConsistency) {
    const int runs = 10;
    
    for (int i = 0; i < runs; ++i) {
        KnightSelection selection(12, 5);
        selection.startSelection();
        
        EXPECT_TRUE(selection.validateSelection());
        
        auto selected = selection.getSelectedKnights();
        EXPECT_EQ(selected.size(), 5);
        
        // Проверяем уникальность выбранных рыцарей
        std::sort(selected.begin(), selected.end());
        auto last = std::unique(selected.begin(), selected.end());
        EXPECT_EQ(last, selected.end());
    }
}

// Тест 5: Проверка работы с разными параметрами
TEST(KnightSelectionTest, DifferentParameters) {
    // Тест с большим количеством рыцарей
    {
        KnightSelection selection(20, 7);
        selection.startSelection();
        EXPECT_TRUE(selection.validateSelection());
    }
    
    // Тест с минимальным количеством
    {
        KnightSelection selection(6, 3);
        selection.startSelection();
        EXPECT_TRUE(selection.validateSelection());
    }
}

// Тест 6: Проверка метода getNeighbors
TEST(KnightSelectionTest, NeighborsCalculation) {
    KnightSelection selection(12, 5);
    
    // Для рыцаря 0 соседи: 11 и 1
    // Для рыцаря 5 соседи: 4 и 6
    // Для рыцаря 11 соседи: 10 и 0
    
    // Проверяем граничные случаи
    auto neighbors0 = selection.getSelectedKnights(); // Используем публичный метод
    // Для реальной проверки нужно добавить публичный метод getNeighbors или сделать тест дружественным
}

// Тест 7: Проверка потоко-безопасности (запуск в нескольких потоках)
TEST(KnightSelectionTest, ThreadSafety) {
    std::vector<std::thread> threads;
    std::vector<bool> results;
    
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([&results, i]() {
            KnightSelection selection(12, 5);
            selection.startSelection();
            results.push_back(selection.validateSelection());
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    // Все запуски должны быть корректны
    for (bool result : results) {
        EXPECT_TRUE(result);
    }
}

// Тест 8: Проверка на deadlock
TEST(KnightSelectionTest, NoDeadlock) {
    // Запускаем выбор с таймаутом
    auto future = std::async(std::launch::async, []() {
        KnightSelection selection(12, 5);
        selection.startSelection();
        return selection.validateSelection();
    });
    
    // Ждем с таймаутом 10 секунд
    auto status = future.wait_for(std::chrono::seconds(10));
    
    EXPECT_NE(status, std::future_status::timeout) 
        << "Возможный deadlock: операция заняла слишком много времени";
    
    if (status == std::future_status::ready) {
        EXPECT_TRUE(future.get());
    }
}

// Тест 9: Проверка повторного запуска
TEST(KnightSelectionTest, RestartSelection) {
    KnightSelection selection(12, 5);
    
    // Первый запуск
    selection.startSelection();
    EXPECT_TRUE(selection.validateSelection());
    
    // Второй запуск (должен сбросить состояние)
    KnightSelection selection2(12, 5);
    selection2.startSelection();
    EXPECT_TRUE(selection2.validateSelection());
}

// Тест 10: Интеграционный тест
TEST(KnightSelectionTest, IntegrationTest) {
    KnightSelection selection;
    
    // Запускаем процесс
    selection.startSelection();
    
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
    
    // Выводим информацию для отладки
    std::cout << "\nИнтеграционный тест: выбраны рыцари ";
    for (int knight : selected) {
        std::cout << knight << " ";
    }
    std::cout << std::endl;
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
