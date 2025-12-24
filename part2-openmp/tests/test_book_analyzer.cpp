#include "book_analyzer.hpp"
#include <gtest/gtest.h>

TEST(BookAnalyzerTest, ASCIILetterDetection) {
    // Тестируем статические методы для ASCII букв
    EXPECT_TRUE(BookAnalyzer::isRussianLetter('A'));
    EXPECT_TRUE(BookAnalyzer::isRussianLetter('Z'));
    EXPECT_TRUE(BookAnalyzer::isRussianLetter('a'));
    EXPECT_TRUE(BookAnalyzer::isRussianLetter('z'));
    
    EXPECT_FALSE(BookAnalyzer::isRussianLetter('1'));
    EXPECT_FALSE(BookAnalyzer::isRussianLetter(' '));
    EXPECT_FALSE(BookAnalyzer::isRussianLetter('@'));
}

TEST(BookAnalyzerTest, ToLowerASCII) {
    EXPECT_EQ(BookAnalyzer::toLowerRussian('A'), 'a');
    EXPECT_EQ(BookAnalyzer::toLowerRussian('Z'), 'z');
    EXPECT_EQ(BookAnalyzer::toLowerRussian('a'), 'a');
    EXPECT_EQ(BookAnalyzer::toLowerRussian('z'), 'z');
    EXPECT_EQ(BookAnalyzer::toLowerRussian('1'), '1');
}

TEST(BookAnalyzerTest, AnalyzeSimpleText) {
    BookAnalyzer analyzer;
    
    // Тест с русским текстом в UTF-8
    std::string testText = "Привет мир";
    auto result = analyzer.analyzeText(testText, 1);
    
    // Должно быть больше 0 русских букв
    EXPECT_GT(result.totalLetters, 0);
    EXPECT_EQ(result.totalCharacters, testText.length());
}

TEST(BookAnalyzerTest, UTF8RussianText) {
    BookAnalyzer analyzer;
    
    // Текст с русскими буквами в разных регистрах
    std::string testText = "Алексей Фёдорович Карамазов";
    auto result = analyzer.analyzeText(testText, 1);
    
    EXPECT_GT(result.totalLetters, 0);
    EXPECT_EQ(result.totalCharacters, testText.length());
}

TEST(BookAnalyzerTest, DifferentThreadCounts) {
    BookAnalyzer analyzer;
    
    std::string testText = "Тестовый текст на русском языке для проверки многопоточности. ";
    // Повторим текст несколько раз для лучшего тестирования
    std::string repeatedText;
    for (int i = 0; i < 50; ++i) {
        repeatedText += testText;
    }
    
    // Тестируем с разным количеством потоков
    std::vector<int> threadCounts = {1, 2};
    
    std::map<std::string, int> firstResult;
    
    for (int threads : threadCounts) {
        auto result = analyzer.analyzeText(repeatedText, threads);
        
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
    for (int i = 0; i < 5000; ++i) {
        largeText += "Быстрая коричневая лиса прыгает через ленивую собаку. ";
    }
    
    auto result1 = analyzer.analyzeText(largeText, 1);
    auto result2 = analyzer.analyzeText(largeText, 2);
    
    EXPECT_GT(result1.totalLetters, 0);
    EXPECT_GT(result2.totalLetters, 0);
    EXPECT_EQ(result1.totalLetters, result2.totalLetters);
}

TEST(BookAnalyzerTest, TestTextFunction) {
    // Тестируем создание тестового текста
    std::string testText = BookAnalyzer::createTestText();
    EXPECT_FALSE(testText.empty());
    EXPECT_GT(testText.length(), 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
