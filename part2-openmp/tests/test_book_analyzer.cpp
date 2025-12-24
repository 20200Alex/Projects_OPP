#include "book_analyzer.hpp"
#include <gtest/gtest.h>
#include <fstream>

TEST(BookAnalyzerTest, RussianLetterDetection) {
    EXPECT_TRUE(BookAnalyzer::isRussianLetter('а'));
    EXPECT_TRUE(BookAnalyzer::isRussianLetter('Я'));
    EXPECT_TRUE(BookAnalyzer::isRussianLetter('ё'));
    EXPECT_TRUE(BookAnalyzer::isRussianLetter('Ё'));
    
    EXPECT_FALSE(BookAnalyzer::isRussianLetter('a'));
    EXPECT_FALSE(BookAnalyzer::isRussianLetter('Z'));
    EXPECT_FALSE(BookAnalyzer::isRussianLetter('1'));
    EXPECT_FALSE(BookAnalyzer::isRussianLetter(' '));
}

TEST(BookAnalyzerTest, ToLowerRussian) {
    EXPECT_EQ(BookAnalyzer::toLowerRussian('А'), 'а');
    EXPECT_EQ(BookAnalyzer::toLowerRussian('Я'), 'я');
    EXPECT_EQ(BookAnalyzer::toLowerRussian('Ё'), 'ё');
    EXPECT_EQ(BookAnalyzer::toLowerRussian('а'), 'а');  // Уже строчная
    EXPECT_EQ(BookAnalyzer::toLowerRussian('z'), 'z');  // Не русская
}

TEST(BookAnalyzerTest, AnalyzeSimpleText) {
    BookAnalyzer analyzer;
    
    // Простой текст с известными буквами
    std::string testText = "аааббвввгггг";
    auto result = analyzer.analyzeText(testText, 1);
    
    EXPECT_GT(result.totalLetters, 0);
    EXPECT_EQ(result.totalCharacters, testText.length());
    
    // Проверяем частоты
    if (result.letterFrequency.find('а') != result.letterFrequency.end()) {
        EXPECT_GE(result.letterFrequency.at('а'), 3);
    }
    
    if (result.letterFrequency.find('г') != result.letterFrequency.end()) {
        EXPECT_GE(result.letterFrequency.at('г'), 4);
    }
}

TEST(BookAnalyzerTest, CaseInsensitive) {
    BookAnalyzer analyzer;
    
    // Текст в разных регистрах
    std::string testText = "АаБбВвГг";
    auto result = analyzer.analyzeText(testText, 1);
    
    // Все буквы должны быть приведены к нижнему регистру
    EXPECT_EQ(result.letterFrequency.size(), 4);
    
    // Проверяем что 'а' и 'А' считаются вместе
    if (result.letterFrequency.find('а') != result.letterFrequency.end()) {
        EXPECT_EQ(result.letterFrequency.at('а'), 2);
    }
    
    if (result.letterFrequency.find('б') != result.letterFrequency.end()) {
        EXPECT_EQ(result.letterFrequency.at('б'), 2);
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
        
        // Проверяем что результаты согласованы для разных потоков
        if (threads == 1) {
            firstResult = result.letterFrequency;
        } else {
            // Результаты должны быть одинаковыми независимо от числа потоков
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

TEST(BookAnalyzerTest, NonRussianText) {
    BookAnalyzer analyzer;
    
    std::string englishText = "This is English text with no Russian letters";
    auto result = analyzer.analyzeText(englishText, 1);
    
    // Не должно быть русских букв
    EXPECT_EQ(result.totalLetters, 0);
    EXPECT_TRUE(result.letterFrequency.empty());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
