#!/bin/bash

echo "=== MPI City Capture Simulation ==="
echo ""

# Создаем директорию сборки
mkdir -p build
cd build

# Собираем проект
echo "Building project..."
cmake ..
make -j$(nproc)

echo ""
echo "Running simulations..."

# Запускаем с разным количеством процессов
echo ""
echo "1. Running with 6 processes (5 cities + commander):"
mpirun -np 6 ./city_capture_app 5

echo ""
echo "2. Running with 11 processes (10 cities + commander):"
mpirun -np 11 ./city_capture_app 10

echo ""
echo "3. Running with 21 processes (20 cities + commander):"
mpirun -np 21 ./city_capture_app 20

echo ""
echo "=== All simulations complete ==="
