#!/bin/bash

echo "OpenMP Book Analysis - Brothers Karamazov"

# Создаем директорию для сборки
mkdir -p build
cd build

# Собираем проект
echo "Building project..."
cmake .. -DBUILD_TESTS=ON
make -j$(nproc)

echo ""
echo "Running analysis..."

# Ищем файл с книгой
BOOK_FILE=""
POSSIBLE_FILES=(
    "../data/karamazov.txt"
    "../data/test_book.txt"
    "../Достоевский Федор. Том 9. Братья Карамазовы - royallib.ru.txt"
    "Достоевский Федор. Том 9. Братья Карамазовы - royallib.ru.txt"
)

for file in "${POSSIBLE_FILES[@]}"; do
    if [ -f "$file" ]; then
        BOOK_FILE="$file"
        echo "Found book file: $BOOK_FILE"
        break
    fi
done

if [ -z "$BOOK_FILE" ]; then
    echo "WARNING: Book file not found!"
    echo "Creating test file for demonstration..."
    
    # Создаем тестовый файл с русским текстом
    echo -e "Это тестовый текст на русском языке.\nОн содержит все буквы русского алфавита.\nАа Бб Вв Гг Дд Ее Ёё Жж Зз Ии Йй Кк Лл Мм Нн Оо Пп Рр Сс Тт Уу Фф Хх Цц Чч Шш Щщ Ъъ Ыы Ьь Ээ Юю Яя\n" > test_input.txt
    for i in {1..10000}; do
        echo -n "Алексей Фёдорович Карамазов был третьим сыном помещика нашего уезда. " >> test_input.txt
    done
    BOOK_FILE="test_input.txt"
fi

# Запускаем анализ
echo ""
echo "Starting analysis with: $BOOK_FILE"
echo ""

if [ -f "./book_analysis" ]; then
    timeout 60 ./book_analysis "$BOOK_FILE"
    
    echo ""
    echo "Checking generated files..."
    
    if [ -f "letter_frequencies.csv" ]; then
        echo "✓ letter_frequencies.csv generated"
        head -5 letter_frequencies.csv
    fi
    
    if [ -f "benchmark_results.csv" ]; then
        echo "✓ benchmark_results.csv generated"
        cat benchmark_results.csv
    fi
    
    if [ -f "generate_plots.py" ]; then
        echo "✓ generate_plots.py generated"
        echo "To generate plots, run: python3 generate_plots.py"
    fi
else
    echo "ERROR: book_analysis executable not found!"
    exit 1
fi

echo ""
echo "Analysis Complete!"
