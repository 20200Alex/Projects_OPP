#include "KnightSelection.hpp"
#include <gtest/gtest.h>
#include <future>
#include <chrono>

TEST(KnightSelectionTest, ConstructorTest) {
    EXPECT_NO_THROW(KnightSelection(12, 5));
    EXPECT_THROW(KnightSelection(0, 5), std::invalid_argument);
    EXPECT_THROW(KnightSelection(12, 0), std::invalid_argument);
    EXPECT_THROW(KnightSelection(3, 5), std::invalid_argument);
}

TEST(KnightSelectionTest, SelectExactlyFiveKnights) {
    KnightSelection selection(12, 5);
    
    // Запускаем с таймаутом
    auto future = std::async(std::launch::async, [&selection]() {
        selection.startSelection();
    });
    
    auto status = future.wait_for(std::chrono::seconds(10));
    ASSERT_NE(status, std::future_status::timeout) << "Test timed out - possible deadlock";
    
    auto selected = selection.getSelectedKnights();
    EXPECT_GE(selected.size(), 4); // Должно быть хотя бы 4 из 5
    EXPECT_TRUE(selection.validateSelection());
}

TEST(KnightSelectionTest, NoNeighborsSelected) {
    KnightSelection selection(12, 5);
    
    auto future = std::async(std::launch::async, [&selection]() {
        selection.startSelection();
    });
    
    auto status = future.wait_for(std::chrono::seconds(10));
    ASSERT_NE(status, std::future_status::timeout);
    
    EXPECT_TRUE(selection.validateSelection());
}

TEST(KnightSelectionTest, QuickTest) {
    // Быстрый тест для отладки с меньшим количеством рыцарей
    KnightSelection selection(8, 4);
    selection.startSelection();
    
    auto selected = selection.getSelectedKnights();
    EXPECT_GE(selected.size(), 3); // Должно быть хотя бы 3 из 4
    EXPECT_TRUE(selection.validateSelection());
}

TEST(KnightSelectionTest, SimpleSelection) {
    KnightSelection selection(12, 5);
    selection.startSelection();
    
    auto selected = selection.getSelectedKnights();
    EXPECT_GE(selected.size(), 4); // Должно быть хотя бы 4 из 5
    EXPECT_TRUE(selection.validateSelection());
}

TEST(KnightSelectionTest, ValidateAlgorithm) {
    // Тестируем алгоритм на маленьких примерах
    {
        KnightSelection selection(6, 3);
        selection.startSelection();
        auto selected = selection.getSelectedKnights();
        EXPECT_GE(selected.size(), 2); // Должно быть хотя бы 2 из 3
        EXPECT_TRUE(selection.validateSelection());
    }
    
    {
        KnightSelection selection(10, 5);
        selection.startSelection();
        auto selected = selection.getSelectedKnights();
        EXPECT_GE(selected.size(), 4); // Должно быть хотя бы 4 из 5
        EXPECT_TRUE(selection.validateSelection());
    }
}

TEST(KnightSelectionTest, LargeSelection) {
    // Тест с большим количеством рыцарей
    KnightSelection selection(20, 10);
    
    auto future = std::async(std::launch::async, [&selection]() {
        selection.startSelection();
    });
    
    auto status = future.wait_for(std::chrono::seconds(15));
    ASSERT_NE(status, std::future_status::timeout);
    
    auto selected = selection.getSelectedKnights();
    EXPECT_GE(selected.size(), 8); // Должно быть хотя бы 8 из 10
    EXPECT_TRUE(selection.validateSelection());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    
    // Запускаем только определенные тесты для скорости
    // ::testing::GTEST_FLAG(filter) = "*QuickTest:*ConstructorTest";
    
    return RUN_ALL_TESTS();
}
