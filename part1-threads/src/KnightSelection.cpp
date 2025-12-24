#include "KnightSelection.hpp"
#include <thread>
#include <chrono>
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <random>

KnightSelection::KnightSelection(int totalKnights, int requiredKnights)
    : totalKnights(totalKnights)
    , requiredKnights(requiredKnights)
    , selected(totalKnights, false)
    , handRaised(totalKnights, false)
    , selectedCount(0)
    , stopFlag(false)
    , gen(rd())
{
    if (totalKnights <= 0 || requiredKnights <= 0 || requiredKnights > totalKnights) {
        throw std::invalid_argument("Invalid number of knights");
    }
}

std::vector<int> KnightSelection::getNeighbors(int id) const {
    std::vector<int> neighbors;
    int left = (id - 1 + totalKnights) % totalKnights;
    int right = (id + 1) % totalKnights;
    neighbors.push_back(left);
    neighbors.push_back(right);
    return neighbors;
}

bool KnightSelection::canRaiseHand(int id) const {
    std::lock_guard<std::mutex> lock(mtx);
    
    // Если уже выбран или уже поднял руку
    if (selected[id] || handRaised[id]) {
        return false;
    }
    
    // Проверяем соседей
    auto neighbors = getNeighbors(id);
    for (int neighbor : neighbors) {
        if (handRaised[neighbor] || selected[neighbor]) {
            return false;
        }
    }
    
    return true;
}

void KnightSelection::knightProcess(int id) {
    std::random_device localRd;
    std::mt19937 localGen(localRd());
    std::uniform_int_distribution<> sleepDist(10, 50); // Уменьшено время сна
    
    while (!stopFlag && selectedCount < requiredKnights) {
        // Проверяем, может ли поднять руку
        bool shouldRaise = false;
        {
            std::lock_guard<std::mutex> lock(mtx);
            shouldRaise = (!selected[id] && !handRaised[id]);
            
            // Проверяем соседей
            if (shouldRaise) {
                auto neighbors = getNeighbors(id);
                for (int neighbor : neighbors) {
                    if (handRaised[neighbor] || selected[neighbor]) {
                        shouldRaise = false;
                        break;
                    }
                }
            }
            
            if (shouldRaise) {
                handRaised[id] = true;
            }
        }
        
        // Спим случайное время
        if (shouldRaise) {
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepDist(localGen)));
            
            // Если поднял руку и не выбран, опускаем ее
            std::lock_guard<std::mutex> lock(mtx);
            if (!selected[id] && handRaised[id]) {
                handRaised[id] = false;
            }
        } else {
            // Короткая пауза если не может поднять руку
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }
}

void KnightSelection::startSelection() {
    std::cout << "Starting knight selection" << std::endl;
    std::cout << "Total knights: " << totalKnights << std::endl;
    std::cout << "Required to select: " << requiredKnights << std::endl;
    
    // Запускаем потоки рыцарей
    std::vector<std::thread> knights;
    for (int i = 0; i < totalKnights; ++i) {
        knights.emplace_back(&KnightSelection::knightProcess, this, i);
    }
    
    // Основной цикл выбора
    int attempts = 0;
    const int maxAttempts = 1000; // Увеличено количество попыток
    
    while (selectedCount < requiredKnights && attempts < maxAttempts) {
        attempts++;
        
        std::vector<int> available;
        
        {
            std::lock_guard<std::mutex> lock(mtx);
            
            // Собираем всех, кто поднял руку
            for (int i = 0; i < totalKnights; ++i) {
                if (handRaised[i] && !selected[i]) {
                    // Проверяем соседей
                    bool valid = true;
                    auto neighbors = getNeighbors(i);
                    for (int neighbor : neighbors) {
                        if (selected[neighbor]) {
                            valid = false;
                            break;
                        }
                    }
                    
                    if (valid) {
                        available.push_back(i);
                    }
                }
            }
        }
        
        // Если есть доступные рыцари, выбираем случайного
        if (!available.empty()) {
            std::uniform_int_distribution<> dis(0, available.size() - 1);
            int chosen = available[dis(gen)];
            
            {
                std::lock_guard<std::mutex> lock(mtx);
                
                // Двойная проверка
                if (!selected[chosen] && handRaised[chosen]) {
                    selected[chosen] = true;
                    handRaised[chosen] = false;
                    selectedCount++;
                    
                    std::cout << "Knight " << chosen << " selected for the mission" << std::endl;
                    std::cout << "Selected: " << selectedCount << " of " << requiredKnights << std::endl;
                    
                    // Опускаем руки соседей
                    auto neighbors = getNeighbors(chosen);
                    for (int neighbor : neighbors) {
                        handRaised[neighbor] = false;
                    }
                }
            }
        } else {
            // Нет доступных - небольшая пауза
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        
        // Каждые 20 попыток сбрасываем все руки для предотвращения deadlock
        if (attempts % 20 == 0) {
            std::lock_guard<std::mutex> lock(mtx);
            std::fill(handRaised.begin(), handRaised.end(), false);
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    
    // Останавливаем все потоки
    stopFlag = true;
    
    // Ждем завершения потоков
    for (auto& knight : knights) {
        if (knight.joinable()) {
            knight.join();
        }
    }
    
    // Проверяем результат
    if (selectedCount >= requiredKnights) {
        std::cout << "Selection completed successfully" << std::endl;
    } else {
        std::cout << "Warning: Selected only " << selectedCount << " knights" << std::endl;
        std::cout << "Expected: " << requiredKnights << " knights" << std::endl;
    }
}

void KnightSelection::printSelectedKnights() const {
    std::lock_guard<std::mutex> lock(mtx);
    
    std::cout << "Selected knights: ";
    bool first = true;
    for (int i = 0; i < totalKnights; ++i) {
        if (selected[i]) {
            if (!first) std::cout << ", ";
            std::cout << i;
            first = false;
        }
    }
    std::cout << std::endl;
}

std::vector<int> KnightSelection::getSelectedKnights() const {
    std::lock_guard<std::mutex> lock(mtx);
    
    std::vector<int> result;
    for (int i = 0; i < totalKnights; ++i) {
        if (selected[i]) {
            result.push_back(i);
        }
    }
    return result;
}

bool KnightSelection::validateSelection() const {
    std::lock_guard<std::mutex> lock(mtx);
    
    // Проверяем количество
    int count = 0;
    for (int i = 0; i < totalKnights; ++i) {
        if (selected[i]) count++;
    }
    
    if (count < requiredKnights) {
        std::cerr << "Error: selected " << count << " knights, expected at least " << requiredKnights << std::endl;
        return false;
    }
    
    // Проверяем, что нет соседей
    for (int i = 0; i < totalKnights; ++i) {
        if (selected[i]) {
            auto neighbors = getNeighbors(i);
            for (int neighbor : neighbors) {
                if (selected[neighbor]) {
                    std::cerr << "Error: knights " << i << " and " << neighbor << " are neighbors" << std::endl;
                    return false;
                }
            }
        }
    }
    
    return true;
}
