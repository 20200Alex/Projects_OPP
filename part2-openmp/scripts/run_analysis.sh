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
    "../data/Достоевский Федор. Том 9. Братья Карамазовы - royallib.ru.txt"
    "../../Достоевский Федор. Том 9. Братья Карамазовы - royallib.ru.txt"
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
    echo "ERROR: Book file not found!"
    echo "Please place 'Достоевский Федор. Том 9. Братья Карамазовы - royallib.ru.txt' in data/ directory"
    exit 1
fi

# Запускаем анализ
echo ""
echo "Starting analysis with book: $BOOK_FILE"
echo "This may take several minutes depending on file size..."
echo ""

timeout 300 ./book_analysis "$BOOK_FILE"

echo ""
echo "Generating plots..."

if [ -f "generate_plots.py" ]; then
    python3 generate_plots.py
else
    echo "Plot script not generated"
fi

echo ""
echo "Analysis Complete!"
echo "Check the following files:"
echo "  - letter_frequencies.csv"
echo "  - benchmark_results.csv"
echo "  - openmp_performance_analysis.png"
echo "  - letter_frequency_analysis.png"
