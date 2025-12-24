#include "KnightSelection.hpp"
#include <gtest/gtest.h>
#include <vector>
#include <algorithm>
#include <future>

TEST(KnightSelectionTest, ConstructorTest) {
    EXPECT_NO_THROW(KnightSelection(12, 5));
    EXPECT_THROW(KnightSelection(0, 5), std::invalid_argument);
    EXPECT_THROW(KnightSelection(12, 0), std::invalid_argument);
    EXPECT_THROW(KnightSelection(3, 5), std::invalid_argument);
}

TEST(KnightSelectionTest, SelectExactlyFiveKnights) {
    KnightSelection selection(12, 5);
    selection.startSelection();
    
    auto selected = selection.getSelectedKnights();
    EXPECT_EQ(selected.size(), 5);
}

TEST(KnightSelectionTest, NoNeighborsSelected) {
    KnightSelection selection(12, 5);
    selection.startSelection();
    
    EXPECT_TRUE(selection.validateSelection());
}

TEST(KnightSelectionTest, MultipleRunsConsistency) {
    const int runs = 3;
    
    for (int i = 0; i < runs; ++i) {
        KnightSelection selection(12, 5);
        selection.startSelection();
        
        EXPECT_TRUE(selection.validateSelection());
        
        auto selected = selection.getSelectedKnights();
        EXPECT_EQ(selected.size(), 5);
        
        std::sort(selected.begin(), selected.end());
        auto last = std::unique(selected.begin(), selected.end());
        EXPECT_EQ(last, selected.end());
    }
}

TEST(KnightSelectionTest, DifferentParameters) {
    {
        KnightSelection selection(20, 7);
        selection.startSelection();
        EXPECT_TRUE(selection.validateSelection());
    }
    
    {
        KnightSelection selection(6, 3);
        selection.startSelection();
        EXPECT_TRUE(selection.validateSelection());
    }
}

TEST(KnightSelectionTest, ThreadSafety) {
    std::vector<std::future<bool>> futures;
    
    for (int i = 0; i < 3; ++i) {
        futures.push_back(std::async(std::launch::async, []() {
            KnightSelection selection(12, 5);
            selection.startSelection();
            return selection.validateSelection();
        }));
    }
    
    for (size_t i = 0; i < futures.size(); ++i) {
        bool result = futures[i].get();
        EXPECT_TRUE(result);
    }
}

TEST(KnightSelectionTest, NoDeadlock) {
    auto future = std::async(std::launch::async, []() {
        KnightSelection selection(12, 5);
        selection.startSelection();
        return selection.validateSelection();
    });
    
    auto status = future.wait_for(std::chrono::seconds(10));
    
    ASSERT_NE(status, std::future_status::timeout);
    
    if (status == std::future_status::ready) {
        EXPECT_TRUE(future.get());
    }
}

TEST(KnightSelectionTest, IntegrationTest) {
    KnightSelection selection;
    
    selection.startSelection();
    
    auto selected = selection.getSelectedKnights();
    
    EXPECT_EQ(selected.size(), 5);
    EXPECT_TRUE(selection.validateSelection());
    
    for (int knight : selected) {
        EXPECT_GE(knight, 0);
        EXPECT_LT(knight, 12);
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
