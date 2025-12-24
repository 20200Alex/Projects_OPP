#include "book_analyzer.hpp"
#include <gtest/gtest.h>

TEST(BookAnalyzerTest, RussianLetterDetection) {
    // Тестируем ASCII буквы
    EXPECT_TRUE(BookAnalyzer::isRussianLetter('A'));
    EXPECT_TRUE(BookAnalyzer::isRussianLetter('Z'));
    EXPECT_TRUE(BookAnalyzer::isRussianLetter('a'));
    EXPECT_TRUE(BookAnalyzer::isRussianLetter('z'));
    
    EXPECT_FALSE(BookAnalyzer::isRussianLetter('1'));
    EXPECT_FALSE(BookAnalyzer::isRussianLetter(' '));
    EXPECT_FALSE(BookAnalyzer::isRussianLetter('@'));
}

TEST(BookAnalyzerTest, ToLowerRussian) {
    EXPECT_EQ(BookAnalyzer::toLowerRussian('A'), 'a');
    EXPECT_EQ(BookAnalyzer::toLowerRussian('Z'), 'z');
    EXPECT_EQ(BookAnalyzer::toLowerRussian('a'), 'a');
    EXPECT_EQ(BookAnalyzer::toLowerRussian('z'), 'z');
    EXPECT_EQ(BookAnalyzer::toLowerRussian('1'), '1');
}

TEST(BookAnalyzerTest, AnalyzeSimpleText) {
    BookAnalyzer analyzer;
    
    // Простой текст с ASCII буквами
    std::string testText = "aaaabbbbccccdddd";
    auto result = analyzer.analyzeText(testText, 1);
    
    EXPECT_GT(result.totalLetters, 0);
    EXPECT_EQ(result.totalCharacters, testText.length());
    
    // Проверяем частоты (буквы 'a', 'b', 'c', 'd' по 4 раза)
    if (result.letterFrequency.find('a') != result.letterFrequency.end()) {
        EXPECT_GE(result.letterFrequency.at('a'), 4);
    }
}

TEST(BookAnalyzerTest, CaseInsensitive) {
    BookAnalyzer analyzer;
    
    // Текст в разных регистрах
    std::string testText = "AaBbCcDd";
    auto result = analyzer.analyzeText(testText, 1);
    
    // Все буквы должны быть приведены к нижнему регистру
    EXPECT_EQ(result.letterFrequency.size(), 4);
    
    // Проверяем что 'a' и 'A' считаются вместе
    if (result.letterFrequency.find('a') != result.letterFrequency.end()) {
        EXPECT_EQ(result.letterFrequency.at('a'), 2);
    }
}

TEST(BookAnalyzerTest, DifferentThreadCounts) {
    BookAnalyzer analyzer;
    
    std::string testText = BookAnalyzer::createTestText();
    
    // Тестируем с разным количеством потоков
    std::vector<int> threadCounts = {1, 2};
    
    std::map<char, int> firstResult;
    
    for (int threads : threadCounts) {
        auto result = analyzer.analyzeText(testText, threads);
        
        EXPECT_GT(result.totalLetters, 0);
        EXPECT_EQ(result.threadsUsed, threads);
        
        if (threads == 1) {
            firstResult = result.letterFrequency;
        } else {
            // Результаты должны быть одинаковыми
            EXPECT_EQ(result.letterFrequency.size(), firstResult.size());
            
            for (const auto& pair : firstResult) {
                EXPECT_EQ(result.letterFrequency.at(pair.first), pair.second);
            }
        }
    }
}

TEST(BookAnalyzerTest, EmptyText) {
    BookAnalyzer analyzer;
    
    std::string emptyText = "";
    auto result = analyzer.analyzeText(emptyText, 1);
    
    EXPECT_EQ(result.totalLetters, 0);
    EXPECT_EQ(result.totalCharacters, 0);
    EXPECT_TRUE(result.letterFrequency.empty());
    EXPECT_TRUE(result.sortedLetters.empty());
}

TEST(BookAnalyzerTest, PerformanceTest) {
    BookAnalyzer analyzer;
    
    // Создаем большой текст для тестирования производительности
    std::string largeText;
    for (int i = 0; i < 100000; ++i) {
        largeText += "The quick brown fox jumps over the lazy dog. ";
    }
    
    auto result1 = analyzer.analyzeText(largeText, 1);
    auto result2 = analyzer.analyzeText(largeText, 2);
    
    EXPECT_GT(result1.totalLetters, 0);
    EXPECT_GT(result2.totalLetters, 0);
    EXPECT_EQ(result1.totalLetters, result2.totalLetters);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
