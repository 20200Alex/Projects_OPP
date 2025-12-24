#!/bin/bash

echo "=== OpenMP Integration Benchmark ==="
echo ""

# Создаем директорию для результатов
mkdir -p results
cd build

# Запускаем с разными конфигурациями
echo "Running benchmarks with different thread counts..."
echo ""

# Разные размеры сеток
SEGMENTS_LIST="1000000 5000000 10000000 20000000"

for SEGMENTS in $SEGMENTS_LIST; do
    echo "Segments: $SEGMENTS"
    echo "------------------------"
    
    # Запускаем с разным количеством потоков
    for THREADS in 1 2 4 8; do
        echo -n "Threads $THREADS: "
        ./integral_app $SEGMENTS $THREADS 2>&1 | grep "Time:" || true
    done
    
    echo ""
done

echo "Benchmark completed!"
echo "Results saved in results/ directory"
